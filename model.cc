
#include <cstdlib>
#include <vector>
#include <random>
#include <array>
#include <cassert>

#ifdef DEBUG_MODE
#define LOG(_f, ...) printf(_f, __VA_ARGS__)
#else
#define LOG(_f, ...)
#endif

std::default_random_engine re;
struct Team
{
    const char* name;
    double strength;
    double advantage;
    double prog;
};

// first of group, second of group
std::pair<size_t, size_t> final16[] = {
    {1, 2},
    {3, 4},

    {2, 1},
    {4, 3},

    {5, 6},
    {7, 8},

    {6, 5},
    {8, 7}
};

enum Result
{
    WIN_A = 1,
    DRAW = 2,
    WIN_B = 3
};

Result SimulateMatch(Team& a, Team& b, bool allowDraw=false)
{
    double aWeight = a.strength * a.advantage;
    double bWeight = b.strength * b.advantage;
    double drawWeight = allowDraw ? ((2 *aWeight * bWeight) / (aWeight + bWeight)) : 0; // harmonic mean
    double lower_bound = 0;
    double upper_bound = (drawWeight + aWeight + bWeight);
    std::uniform_real_distribution<double> unif(lower_bound,upper_bound);
    auto matchResult = unif(re);
    Result result = WIN_B;
    const char* winner = b.name;
    if(matchResult <= aWeight)
    {
        result = WIN_A;
        a.advantage += 0.1;
        winner = a.name;
    }
    else if(matchResult <= (aWeight+drawWeight))
    {
        result = DRAW;
        winner = "draw";
    }
    else
    {
        b.advantage += 0.1;
    }

    LOG("match %s vs. %s winner %s. Info %f %f %f %f %f\n", a.name, b.name, winner, aWeight, bWeight, drawWeight, upper_bound, matchResult);
    return result;
}

std::pair<size_t, size_t> SimulateGroup(Team* group, size_t offset)
{
    double points[4];
    double leaderPoints=0.0;
    double secondPoints=0.0;
    size_t leader=0;
    size_t second=0;
    for(int i = 0; i < 4; ++i)
    {
        for(int k = i+1; k < 4; ++k)
        {
            switch(SimulateMatch(group[i], group[k], true))
            {
                case WIN_A: points[i] += 3; break;
                case WIN_B: points[k] += 3; break;
                case DRAW:
                {
                    points[i] += 1;
                    points[k] += 1;
                }
                break;
            }
        }
    }
    for(size_t i = 0; i < 4; ++i)
    {
        if(leaderPoints < points[i])
        {
            second = leader;
            secondPoints = leaderPoints;
            leader = i;
            leaderPoints = points[i];
        }
        else if(secondPoints < points[i])
        {
            second = i;
            secondPoints = points[i];
        }
    }
    return std::make_pair(leader+offset, second+offset);
}

const std::array<Team, 32> goldenGroups =
{
    // A
    Team{"Russia",       766.0, 1.0, 0.0},
    Team{"SaudiArabia",  49.0,  1.0, 0.0},
    Team{"Egypt",        674.0, 1.0, 0.0},
    Team{"Uruguay",      771.0, 1.0, 0.0},
    //               B    B
    Team{"Portugal",     774.0, 1.0, 0.0},
    Team{"Spain",        793.0, 1.0, 0.0},
    Team{"Morocco",      299.0, 1.0, 0.0},
    Team{"Iran",         299.0, 1.0, 0.0},
    //               C    C
    Team{"France",       793.0, 1.0, 0.0},
    Team{"Australia",    499.0, 1.0, 0.0},
    Team{"Peru",         599.0, 1.0, 0.0},
    Team{"Denmark",      719.0, 1.0, 0.0},
    //               D    D
    Team{"Argentina",    790.0, 1.0, 0.0},
    Team{"Iceland",      549.0, 1.0, 0.0},
    Team{"Croatia",      766.0, 1.0, 0.0},
    Team{"Nigeria",      599.0, 1.0, 0.0},
    //               E    E
    Team{"Brazil",       795.0, 1.0, 0.0},
    Team{"Switzerland",  699.0, 1.0, 0.0},
    Team{"CostaRica",    299.0, 1.0, 0.0},
    Team{"Serbia",       599.0, 1.0, 0.0},
    //               F    F
    Team{"Germany",      795.0, 1.0, 0.0},
    Team{"Mexico",       699.0, 1.0, 0.0},
    Team{"Sweden",       649.0, 1.0, 0.0},
    Team{"SouthKorea",   299.0, 1.0, 0.0},
    //               G    G
    Team{"Belgium",      789.0, 1.0, 0.0},
    Team{"Panama",       49.0,  1.0, 0.0},
    Team{"Tunisia",      49.0,  1.0, 0.0},
    Team{"England",      783.0, 1.0, 0.0},
    //               H    H
    Team{"Poland",       749.0, 1.0, 0.0},
    Team{"Senegal",      599.0, 1.0, 0.0},
    Team{"Colombia",     759.0, 1.0, 0.0},
    Team{"Japan",        549.0, 1.0, 0.0}
};

int main()
{
    std::array<Team, 32> finalResults = goldenGroups;
    for(int simCount = 0; simCount < 1; ++simCount)
    {
        std::array<Team, 32> groups = goldenGroups;
        // simulate groups
        std::vector<size_t> groupResults;
        char letter = 'A';
        for(size_t i = 0; i < sizeof(groups) / sizeof(groups[0]); i += 4)
        {
            LOG("START GROUP %c\n", letter++);
            auto res = SimulateGroup(&groups[i], i);
            groupResults.push_back(res.first);
            groupResults.push_back(res.second);
        }
        assert(groupResults.size() == 16);
        // groupResults are the groups index for first, second of each group

        std::vector<size_t> final16Results;
        int cnt = 0;
        for(size_t i = 0; i < sizeof(final16) / sizeof(final16[0]); i += 1)
        {
            LOG("START FINAL 16 %d\n", ++cnt);
            size_t aId = 2*(final16[i].first-1);
            size_t bId = 2*(final16[i].second-1)+1;
            Team& a = groups[groupResults[aId]];
            Team& b = groups[groupResults[bId]];
            a.prog += 5;
            b.prog += 5;
            final16Results.push_back(SimulateMatch(a, b) == WIN_A ? groupResults[aId] : groupResults[bId]);
        }
        assert(final16Results.size() == 8);

        // play quarters
        std::vector<size_t> qtrResults;
        letter = 'A';
        for(size_t i = 0; i < final16Results.size(); i += 2)
        {
            LOG("START Quarters %c\n", letter++);
            size_t aId = final16Results[i];
            size_t bId = final16Results[i+1];
            Team& a = groups[aId];
            Team& b = groups[bId];
            a.prog += 10;
            b.prog += 10;
            qtrResults.push_back(SimulateMatch(a, b) == WIN_A ? aId : bId);
        }
        assert(qtrResults.size() == 4);

        // play semi
        std::vector<size_t> semiResults;
        cnt = 0;
        for(size_t i = 0; i < qtrResults.size(); i += 2)
        {
            LOG("START Semi %d\n", ++cnt);
            size_t aId = qtrResults[i];
            size_t bId = qtrResults[i+2];
            Team& a = groups[aId];
            Team& b = groups[bId];
            a.prog += 15;
            b.prog += 15;
            semiResults.push_back(SimulateMatch(a, b) == WIN_A ? aId : bId);
        }
        assert(semiResults.size() == 2);

        // play finals
        {
            LOG("START FINAL\n", 1);
            Team& a = groups[semiResults[0]];
            Team& b = groups[semiResults[1]];
            a.prog += 20;
            b.prog += 20;
            auto winner = SimulateMatch(a, b) == WIN_A ? a : b;
        }
        
        cnt = 0;
        for(auto team : groups)
        {
            LOG("Team %s\t\t%f\n", team.name, team.prog);
            finalResults[cnt].prog = ((finalResults[cnt].prog*(simCount)) + team.prog) / (simCount+1);
            ++cnt;
        }
    }
    for(auto team : finalResults)
    {
        printf("Team %s\t\t%f\n", team.name, team.prog);
    }
}
