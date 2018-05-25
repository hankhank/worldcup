
#include <cstdlib>
#include <vector>
#include <map>

struct Team
{
    std::string country;
    double strength;
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
    WIN_A,
    DRAW,
    WIN_B
};

Result SimulateMatch(const Team& a, const Team& b)
{
    return (std::rand() % (a.strength + b.strength)) < a.strength;
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
            switch(SimulateMatch(group[i], group[k]))
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

int main()
{
    Team groupStages[] =
    {
        // A
        {"Russia",       0.0, 0.0},
        {"SaudiArabia",  0.0, 0.0},
        {"Egypt",        0.0, 0.0},
        {"Uruguay",      0.0, 0.0},
        //               B
        {"Portugal",     0.0, 0.0},
        {"Spain",        0.0, 0.0},
        {"Morocco",      0.0, 0.0},
        {"Iran",         0.0, 0.0},
        //               C
        {"France",       0.0, 0.0},
        {"Australia",    0.0, 0.0},
        {"Peru",         0.0, 0.0},
        {"Denmark",      0.0, 0.0},
        //               D
        {"Argentina",    0.0, 0.0},
        {"Iceland",      0.0, 0.0},
        {"Croatia",      0.0, 0.0},
        {"Nigeria",      0.0, 0.0},
        //               E
        {"Brazil",       0.0, 0.0},
        {"Switzerland",  0.0, 0.0},
        {"CostaRica",    0.0, 0.0},
        {"Serbia",       0.0, 0.0}
        //               F
        {"Germany",      0.0, 0.0},
        {"Mexico",       0.0, 0.0},
        {"Sweden",       0.0, 0.0},
        {"SouthKorea",   0.0, 0.0},
        //               G
        {"Belgium",      0.0, 0.0},
        {"Panama",       0.0, 0.0},
        {"Tunisia",      0.0, 0.0},
        {"England",      0.0, 0.0},
        //               H
        {"Poland",       0.0, 0.0},
        {"Senegal",      0.0, 0.0},
        {"Colombia",     0.0, 0.0},
        {"Japan",        0.0, 0.0},
    };

    // simulate groups
    std::vector<size_t> groupResults;
    for(size_t i = 0; i < sizeof(groupStages) / sizeof(groupStages[0]); i += 4)
    {
        groupResults.push_back(SimulateGroup(&groupStages[i], i));
    }
    assert(groupResults.size() == 16);

    std::vector<size_t> final16Results;
    for(size_t i = 0; i < sizeof(final16) / sizeof(final16[0]); i += 4)
    {
        size_t aId = 2*final16[i].first;
        size_t bId = 2*(final16[i].second)+1;
        Team& a = groupResults[aId];
        Team& b = groupResults[bId];
        final16Results.push_back(SimulateMatch(a, b) == WIN_A ? aId : bId);
    }
    assert(groupResults.size() == 8);

    // play quarters
    std::vector<size_t> qtrResults;
    for(size_t i = 0; i < final16Results.size(); i += 2)
    {
        size_t aId = final16Results[i];
        size_t bId = final16Results[i+1];
        Team& a = groupResults[aId];
        Team& b = groupResults[bId];
        qtrResults.push_back(SimulateMatch(a, b) == WIN_A ? aId : bId);
    }
    assert(qtrResults.size() == 4);

    // play semi
    std::vector<size_t> semiResults;
    for(size_t i = 0; i < qtrResults.size(); i += 2)
    {
        size_t aId = qtrResults[i];
        size_t bId = qtrResults[i+2];
        Team& a = groupResults[aId];
        Team& b = groupResults[bId];
        semiResults.push_back(SimulateMatch(a, b) == WIN_A ? aId : bId);
    }
    assert(semiResults.size() == 2);

    // play finals
    {
        Team& a = semiResults[0];
        Team& b = semiResults[1];
        auto winner = SimulateMatch(a, b) == WIN_A ? a : b;
    }

}
