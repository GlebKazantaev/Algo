#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <deque>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <tuple>

using namespace std;

struct Street {
    using Ptr = std::shared_ptr<Street>;
    int64_t start_id{-1}, end_id{-1}, car_time{0}, lights_time{0};
    string name;
    int64_t start_count{0};
    double priority{1.};

    deque<int64_t> cars_queue;

    bool start_street() const { return start_count > 0; }
};

struct Car {
    std::vector<Street::Ptr> streets;
    int64_t remaining_time{0};
    int64_t current_street_id{0};
    bool finished{false};

    Street::Ptr get_current_street() const { return streets[current_street_id]; }

    void go_next_street()
    {
        current_street_id++;
        remaining_time = streets[current_street_id]->car_time;
    }

    bool on_the_last_street() const { return current_street_id == streets.size() - 1; }
};

struct Intersection {
    vector<Street::Ptr> incoming_streets;
    int64_t green_light_id{0};
    int64_t counter{0};

    Street::Ptr get_green_street() const { return incoming_streets[green_light_id]; }

    void increase_timer()
    {
        if (incoming_streets.empty()) return;
        counter++;
        if (counter > incoming_streets[green_light_id]->lights_time)
        {
            cout << "NO!!!";
        }
        if (counter == incoming_streets[green_light_id]->lights_time)
        {
            // TODO: skip for zero times
            green_light_id = green_light_id == incoming_streets.size() - 1 ? 0 : green_light_id + 1;
            counter = 0;
        }
    }

    void sort_by_priorities()
    {
        auto cmp_streets = [&](const Street::Ptr & a, const Street::Ptr & b) -> bool {
            if (a->start_street() && !b->start_street()) {
                return false;
            }
            if (!a->start_street() && b->start_street()) {
                return true;
            }
            if (a->start_street() && b->start_street()) {
                return a->start_count < b->start_count;
            }
            return a->priority < b->priority;
        };
        std::sort(incoming_streets.rbegin(), incoming_streets.rend(), cmp_streets);
    }
};

using Cars = std::vector<Car>;

struct Graph : public std::vector<Intersection> {
    explicit Graph(const size_t & count) : std::vector<Intersection>(count) {}

    void save_answer(const std::string & file_name)
    {
        ofstream out(file_name);
        int64_t res = std::count_if(this->begin(),
                                    this->end(),
                                    [](const Intersection & inter) { return std::any_of(inter.incoming_streets.begin(),
                                                                                        inter.incoming_streets.end(),
                                                                                        [](const Street::Ptr & s) { return s->lights_time > 0;
                                                                                        });
                                    });

        out << res << endl;
        for (int i = 0; i < this->size(); ++i)
        {
            auto cnt = std::count_if(this->at(i).incoming_streets.begin(),
                                     this->at(i).incoming_streets.end(),
                                     [](const Street::Ptr & s) { return s->lights_time > 0; });
            if (cnt == 0) continue;

            out << i << endl;
            out << cnt << endl;
            for (auto & s : this->at(i).incoming_streets)
            {
                out << s->name << " " << s->lights_time << endl;
            }
        }
        out.close();
    }
};

void init_streets_order(Graph & g)
{
    for (auto & i : g)
    {
        for (auto & s : i.incoming_streets)
        {
            i.sort_by_priorities();
            s->lights_time = 1;
        }
    }
}

void reset_state(Graph & g, const Cars & cars)
{
    for (auto & i : g)
    {
        i.counter = 0;
        i.green_light_id = 0;
        for (auto & s : i.incoming_streets)
        {
            s->cars_queue.clear();
        }
    }
    int64_t car_id{0};
    for (auto & car : cars)
    {
        car.streets.front()->cars_queue.push_back(car_id);
        ++car_id;
    }
}

tuple<int64_t, unordered_map<string, vector<int64_t>>> optimize(
                 Graph & g,
                 const unordered_map<string, Street::Ptr> & name_to_street,
                 const Cars & cars_to_go,
                 const int64_t & simulation_time,
                 const int64_t & points_for_finish)
{
    int64_t points{0};
    int64_t elapsed_time{0};
    auto cars = cars_to_go;

    unordered_map<string, vector<int64_t>> waiting;

    while (elapsed_time <= simulation_time)
    {
        unordered_set<int64_t> used_cars;
        int64_t intersection_id{0};
        for (auto & i : g)
        {
            // Update cars status
            for (auto & s : i.incoming_streets)
            {
                if (s->cars_queue.empty()) continue;

                // Check first car in a queue
                auto first_car_id = s->cars_queue.front();
                if (!used_cars.count(first_car_id))
                {
                    used_cars.insert(first_car_id);
                    auto & first_car = cars[first_car_id];
                    if (first_car.remaining_time == 0)
                    {
                        if (i.get_green_street()->name == s->name)
                        {
                            first_car.go_next_street();
                            auto next_street = first_car.get_current_street();
                            next_street->cars_queue.push_back(first_car_id);
                            s->cars_queue.pop_front();
                        }
                        else
                        {
                            waiting[s->name].push_back(elapsed_time);
                        }

                    }
                    else
                    {
                        first_car.remaining_time--;
                        if (first_car.remaining_time == 0)
                        {
                            // finish if we arrived
                            if (first_car.on_the_last_street())
                            {
                                points += points_for_finish + (simulation_time - elapsed_time);
                                first_car.finished = true;
                                s->cars_queue.pop_front();
                            }
                            // push to next road if light is green
                            else if (i.get_green_street()->name == s->name)
                            {
                                first_car.go_next_street();
                                auto next_street = first_car.get_current_street();
                                next_street->cars_queue.push_back(first_car_id);
                                s->cars_queue.pop_front();
                            }
                            else
                            {
                                waiting[s->name].push_back(elapsed_time);
                            }
                        }
                    }
                }

                // Check remaining cars
                for (auto & car_id : s->cars_queue)
                {
                    if (used_cars.count(car_id)) continue;
                    used_cars.insert(car_id);

                    auto &car = cars[car_id];

                    if (car.remaining_time != 0)
                    {
                        car.remaining_time--;
                    }
                }

                // Collect arrived cars and add points
                for(auto & car_id : s->cars_queue)
                {
                    auto & car = cars[car_id];
                    if (car.remaining_time == 0 && car.on_the_last_street())
                    {
                        // make car finish
                        points += points_for_finish + (simulation_time - elapsed_time);
                        car.finished = true;
                    }
                }

                // remove cars that already have finished
                auto & q = s->cars_queue;
                q.erase(std::remove_if(q.begin(),q.end(),
                                       [&cars](const int& id) { return cars[id].finished; }),q.end());

                for (auto & id : s->cars_queue)
                {
                    if (cars[id].finished) cout << "NONONO!!!\n";
                }
            }
            i.increase_timer();
            ++intersection_id;
        }

//        std::cout << "Time: " << elapsed_time << endl;
//        int i = 0;
//        for (auto & car : cars)
//        {
//            //cout << " Car(" << i << ") " << car.get_current_street()->name << " " << car.remaining_time << " " << (car.finished ? "Finished" : "") << endl;
//            ++i;
//        }

        ++elapsed_time;
    }

    int64_t sum_waiting{0};
    for (auto & item : waiting)
    {
        sum_waiting += item.second.size();
    }
    //cout << "Waisted time: " << sum_waiting << endl;

    return {points, waiting};
}

int64_t solve(const string & FILE_IN, const string & FILE_OUT)
{
    //--------------READ DATA---------------
    ifstream in(FILE_IN);
    int64_t D, I, S, V, F;
    in >> D >> I >> S >> V >> F;
    const int64_t max_add{5};

    std::vector<Street::Ptr> streets(S);
    std::unordered_map<string, Street::Ptr> name_to_street;
    for (auto & s : streets)
    {
        s = std::make_shared<Street>();
        in >> s->start_id >> s->end_id >> s->name >> s->car_time;
        name_to_street[s->name] = s;
    }

    Cars cars(V);
    for (int car_id = 0; car_id < cars.size(); ++car_id)
    {
        auto & car = cars[car_id];
        int64_t P;
        in >> P;
        string name;
        for (int j = 0; j < P; ++j)
        {
            in >> name;
            auto s = name_to_street[name];
            car.streets.push_back(s);
            // if (j != P - 1)
            {
                s->priority += 1.;// / (double)P;
            }
            if (j == 0)
            {
                s->start_count += 1;
                s->cars_queue.push_back(car_id);
            }
        }
    }
    in.close();

    Graph g(I);
    for (const auto & s : streets)
    {
        if (s->priority > 1.)
        {
            g[s->end_id].incoming_streets.push_back(s);
        }
    }

    //--------------ALGO----------
    init_streets_order(g);
    int64_t total{0}, prev_points{0};

    for(int64_t i = 0; i < 1000; ++i)
    {
        unordered_map<string, vector<int64_t>> waiting;

        // Calculate total points
        tie(total, waiting) = optimize(g, name_to_street, cars, D, F);
        prev_points = total;
        std::cout << "Total points(" << i << "): " << total << endl;
        g.save_answer(FILE_OUT + "_" + to_string(i) + "_" + to_string(total) + ".txt");
        reset_state(g, cars);

        // Sort intersections using loading
        unordered_map<int64_t, int64_t> inter_to_count;
        for (auto &item : waiting) {
            auto s = name_to_street.at(item.first);
            inter_to_count[s->end_id] += item.second.size();
        }

        vector<pair<int64_t, int64_t>> most_waiting;
        for (auto & item : inter_to_count)
        {
            most_waiting.emplace_back(item.second, item.first);
        }
        sort(most_waiting.rbegin(), most_waiting.rend());

        for (auto & pos : most_waiting)
        {
            auto & inter = g[pos.second];

            // Optimize lights timings
            for(auto & s : inter.incoming_streets)
            {
                s->lights_time += 1;

                tie(total, waiting) = optimize(g, name_to_street, cars, D, F);

                if (total < prev_points)
                {
                    s->lights_time -= 1;
                }
                else if (total > prev_points)
                {
                    g.save_answer(FILE_OUT + "_" + to_string(i) + "_" + to_string(total) + ".txt");
                    std::cout << "Total points LIGHT (" << i << "): " << total << " +" << total - prev_points << endl;
                    prev_points = total;
                }
                reset_state(g, cars);
            }

            // Optimize streets order
            vector<pair<int64_t, Street::Ptr>> order;
            for (auto & s : inter.incoming_streets)
            {
                order.emplace_back(waiting[s->name].size(), s);
            }
            sort(order.rbegin(), order.rend());

            double cur_priority{order.size() / 3.};
            for (auto & item : order)
            {
                auto s = item.second;
                if (cur_priority > 0)
                {
                    s->priority += 1.;
                    --cur_priority;
                }
                else
                {
                    s->priority -= 1.;
                }
            }
            inter.sort_by_priorities();

            tie(total, waiting) = optimize(g, name_to_street, cars, D, F);
            if (total < prev_points)
            {
                // restore priorities
                cur_priority = order.size() / 3.;
                for (auto & item : order)
                {
                    auto s = item.second;
                    if (cur_priority > 0)
                    {
                        s->priority -= 1.;
                        --cur_priority;
                    }
                }
                inter.sort_by_priorities();
            }
            else if (total > prev_points)
            {
                g.save_answer(FILE_OUT + "_" + to_string(i) + "_" + to_string(total) + ".txt");
                std::cout << "Total points ORDER (" << i << "): " << total << " +" << total - prev_points << endl;
                prev_points = total;
            }
            reset_state(g, cars);
        }

        // Update graph
        // Stupid bruteforce
//        {
//            unordered_map<int64_t, vector<pair<int64_t, Street::Ptr>>> wait_map;
//            int64_t sum_waiting{0};
//            for (auto &item : waiting) {
//                auto s = name_to_street.at(item.first);
//                wait_map[s->end_id].emplace_back(item.second.size(), s);
//                sum_waiting += item.second.size();
//            }
//
//            for (auto &item : wait_map) {
//                auto inter_id = item.first;
//                auto &wait_list = item.second;
//
//                double cur_priority{2};
//                int64_t cur_time{0};
//
//                sort(wait_list.rbegin(), wait_list.rend());
//                for (auto &list_item : wait_list) {
//                    auto cur_street = list_item.second;
//                    cur_street->priority += (cur_priority > 0 ? 1 : 0);
//                    cur_street->lights_time = max((int64_t) 1, cur_street->lights_time + (cur_time > 0 ? 1 : -1));
//                    --cur_priority;
//                    --cur_time;
//                }
//                g[inter_id].sort_by_priorities();
//            }
//        }
    }
    return prev_points;
}

int main()
{
    {
        string pref = "/Users/gleb_dmitrievich/CLionProjects/HashCode/tests/";
        vector<string> test_cases{"f"};//{"a", "b", "c", "e", "f"};
        int64_t sum{0};
        for (auto &test : test_cases)
        {
            cout << "Processing: " << test << endl;
            auto start = std::chrono::system_clock::now();
            int64_t res = solve(pref + test + ".txt", pref + test + "_ans");
            sum += res;
            auto end = std::chrono::system_clock::now();
            std::chrono::duration<double> elapsed = end - start;
            cout << "Completed in " << elapsed.count() << "s" << endl << endl;
        }
        cout << "Sum: " << sum << endl;
    }
    return 0;
}
