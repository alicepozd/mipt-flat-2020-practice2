#include <iostream>
#include <utility>
#include <vector>
#include <set>
#include <cassert>


struct Rule {
    Rule(char nonterminal_from, std::string  to) :
            nonterminal_from(nonterminal_from), to(std::move(to)) {}

    char nonterminal_from;
    std::string to;
};


bool operator<(const Rule& first, const Rule& second) {
    return first.nonterminal_from < second.nonterminal_from || (first.nonterminal_from == second.nonterminal_from &&
                                                                first.to < second.to);
}


bool operator==(const Rule& first, const Rule& second) {
    return !(first < second) && !(second < first);
}


struct Situation {
    Situation(Rule  rule, int point_index, int begin_symb_numb) :
            rule(std::move(rule)), point_index(point_index), begin_symb_numb(begin_symb_numb) {}

    Rule rule;
    int point_index;
    int begin_symb_numb;
};


bool operator<(const Situation& first, const Situation& second) {
    return first.rule < second.rule || (first.rule == second.rule && first.point_index < second.point_index ||
                                        (first.point_index == second.point_index && first.begin_symb_numb < second.begin_symb_numb));
}


bool operator==(const Situation& first, const Situation& second) {
    return !(first < second) && !(second < first);
}


class Algo {
public:
    Algo() = default;

    void set_grammar(const std::set<Rule>& new_grammar) {
        grammar = new_grammar;
    }

    bool predict(std::string& word) {
        D.resize(word.size() + 1);
        D[0].insert(Situation(Rule('\0', "S"), 0, 0));
        do {
            int size_before = D[0].size();
            Predict(0);
            Complete(0);
            if (size_before == D[0].size()) {
                break;
            }
        } while (true);
        for (int j = 1; j <= word.size(); ++j) {
            Scan(j - 1, word[j - 1]);
            do {
                int size_before = D[j].size();
                Predict(j);
                Complete(j);
                if (size_before == D[j].size()) {
                    break;
                }
            } while (true);
        }
        return D[word.size()].find(Situation(Rule('\0', "S"), 1, 0)) != D[word.size()].end();
    }

    static void run_tests() {
        Algo algo;

        std::cout << "Predict testing\n";
        algo.D.resize(4);
        algo.grammar.insert(Rule('S', "S"));
        algo.grammar.insert(Rule('S', "aT"));
        algo.grammar.insert(Rule('T', "b"));
        algo.D[1].insert(Situation(Rule('S', "aT"), 1, 0));
        algo.D[1].insert(Situation(Rule('T', "b"), 0, 0));
        algo.D[1].insert(Situation(Rule('S', "S"), 0, 0));
        algo.Predict(1);
        std::set<Situation> correct;
        correct.insert(Situation(Rule('S', "aT"), 1, 0));
        correct.insert(Situation(Rule('T', "b"), 0, 0));
        correct.insert(Situation(Rule('S', "S"), 0, 0));
        correct.insert(Situation(Rule('S', "aT"), 0, 1));
        correct.insert(Situation(Rule('T', "b"), 0, 1));
        correct.insert(Situation(Rule('S', "S"), 0, 1));
        assert(algo.D[1] == correct);
        std::cout << "[OK]\n";

        std::cout << "Scan testing\n";
        algo.Scan(1, 'a');
        correct.clear();
        correct.insert(Situation(Rule('S', "aT"), 1, 1));
        assert(algo.D[2] == correct);
        std::cout << "[OK]\n";

        std::cout << "Complete testing\n";
        algo.D[3].insert(Situation(Rule('T', "b"), 1, 1));
        algo.Complete(3);
        correct.clear();
        correct.insert(Situation(Rule('T', "b"), 1, 1));
        correct.insert(Situation(Rule('S', "aT"), 2, 0));
        assert(algo.D[3] == correct);
        std::cout << "[OK]\n";
    }
private:
    bool Predict(int D_index) {
        std::set<Situation> new_situations;
        for (Situation situation : D[D_index]) {
            for (Rule rule : grammar) {
                if (rule.nonterminal_from == situation.rule.to[situation.point_index]) {
                    Situation new_situation = Situation(rule, 0, D_index);
                    if (D[D_index].find(new_situation) == D[D_index].end()) {
                        new_situations.insert(new_situation);
                    }
                }
            }
        }
        for (Situation situation : new_situations) {
            D[D_index].insert(situation);
        }
        return !new_situations.empty();
    }

    bool Scan(int D_index, char symbol) {
        std::set<Situation> new_situations;
        for (Situation situation : D[D_index]) {
            if (symbol == situation.rule.to[situation.point_index]) {
                Situation new_situation = Situation(situation.rule, situation.point_index + 1, situation.begin_symb_numb);
                if (D[D_index + 1].find(new_situation) == D[D_index + 1].end()) {
                    new_situations.insert(new_situation);
                }
            }
        }
        for (Situation situation : new_situations) {
            D[D_index + 1].insert(situation);
        }
        return !new_situations.empty();
    }

    bool Complete(int D_index) {
        std::set<Situation> new_situations;
        for (const Situation& under_situation : D[D_index]) {
            if (under_situation.point_index != under_situation.rule.to.size()) {
                continue;
            }
            for (const Situation& upper_situation : D[under_situation.begin_symb_numb]) {
                if (upper_situation.rule.to[upper_situation.point_index] == under_situation.rule.nonterminal_from) {
                    Situation new_situation = Situation(upper_situation.rule, upper_situation.point_index + 1, upper_situation.begin_symb_numb);
                    if (D[D_index].find(new_situation) == D[D_index].end()) {
                        new_situations.insert(new_situation);
                    }
                }
            }
        }
        for (Situation situation : new_situations) {
            D[D_index].insert(situation);
        }
        return !new_situations.empty();
    }

    std::vector<std::set<Situation>> D;
    std::set<Rule> grammar;
};


std::string get_to(std::string rule_string) {
    std::string new_string;
    for (int i = 3; i < rule_string.size(); ++i) {
        new_string += rule_string[i];
    }
    return new_string;
}


int main() {
    std::string mode;
    std::cin >> mode;
    if (mode == "run_tests") {
        Algo::run_tests();
        return 0;
    }
    int rule_numb_in_grammar;
    std::cin >> rule_numb_in_grammar;
    std::set<Rule> grammar;
    for (int i = 0; i < rule_numb_in_grammar; ++i) {
        std::string rule_string;
        std::cin >> rule_string;
        Rule new_rule = Rule(rule_string[0], get_to(rule_string));
        grammar.insert(new_rule);
    }
    Algo algo;
    algo.set_grammar(grammar);
    int word_numb;
    std::cin >> word_numb;
    for (int i = 0; i < word_numb; ++i) {
        std::string word;
        std::cin >> word;
        std::cout << algo.predict(word);
    }
    return 0;
}
