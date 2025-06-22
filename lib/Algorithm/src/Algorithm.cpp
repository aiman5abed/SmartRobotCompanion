#include "Algorithm.h"
#include <queue>
#include <tuple>
#include <map>
#include <climits>
#include <cmath>

static const float MOVE_COST = 2.0f;
static const float ROTATION_COST = 1.5f;

Algorithm::Algorithm(House* h, VacuumCleaner* v)
    : currentObjective(CLEANING), currentPath(), house(h), vacuum(v) {
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            obstacleMap[i][j] = false;
        }
    }
}

void Algorithm::setObjective(AlgorithmObjective objective) {
    currentObjective = objective;
    currentPath.clear();
}

std::list<MovementCommand> Algorithm::getCurrentPath() const {
    return currentPath;
}

void Algorithm::calculateNextMove() {
    if (currentObjective == RETURN_HOME) {
        calculateReturnPath();
        return;
    }
    if (!currentPath.empty()) {
        return;
    }

    CellPriority target = findHighestPriorityCell();
    if (target.dirtLevel == 0) {
        currentObjective = RETURN_HOME;
        calculateReturnPath();
    } else {
        currentPath = aStarPathfinding(target.x, target.y);
    }
}

CellPriority Algorithm::findHighestPriorityCell() const {
    CellPriority best{0, 0, 0, -1e6f};
    for (int x = 0; x < GRID_SIZE; ++x) {
        for (int y = 0; y < GRID_SIZE; ++y) {
            if (obstacleMap[x][y]) continue;
            int dirt = house->getDirtLevel(x, y);
            float score = dirt * 100.0f - manhattanDistance(
                vacuum->getX(), vacuum->getY(), x, y);
            if (score > best.priorityScore) {
                best = {x, y, dirt, score};
            }
        }
    }
    return best;
}

void Algorithm::calculateReturnPath() {
    auto home = house->getDockingStation();
    int homeX = home.first;
    int homeY = home.second;
    currentPath = aStarPathfinding(homeX, homeY);
}

std::list<MovementCommand> Algorithm::aStarPathfinding(int targetX, int targetY) {
    struct Node {
        int x, y;
        Direction facing;
        float cost;
        float heuristic;
        std::list<MovementCommand> path;
        bool operator<(const Node& other) const {
            return (cost + heuristic) > (other.cost + other.heuristic);
        }
    };

    auto heurFn = [&](int x1, int y1, int x2, int y2, Direction d) {
        return manhattanDistance(x1, y1, x2, y2) * MOVE_COST;
    };
    auto costFn = [&](Direction f, Command c) {
        return (c == FORWARD) ? MOVE_COST : ROTATION_COST;
    };

    std::priority_queue<Node> openSet;
    std::map<std::tuple<int, int, Direction>, float> visited;

    Node start{vacuum->getX(), vacuum->getY(), vacuum->getDirection(),
               0.0f, heurFn(vacuum->getX(), vacuum->getY(), targetX, targetY, vacuum->getDirection()), {}};
    openSet.push(start);
    visited[{start.x, start.y, start.facing}] = 0.0f;

    while (!openSet.empty()) {
        Node cur = openSet.top(); openSet.pop();
        if (cur.x == targetX && cur.y == targetY) {
            return cur.path;
        }

        for (int cmd = FORWARD; cmd <= RIGHT; ++cmd) {
            MovementCommand m{static_cast<Command>(cmd)};
            int nx = cur.x;
            int ny = cur.y;
            Direction nf = cur.facing;
            if (m.command == FORWARD) {
                switch (cur.facing) {
                    case NORTH: nx--; break;
                    case EAST:  ny++; break;
                    case SOUTH: nx++; break;
                    case WEST:  ny--; break;
                }
            } else if (m.command == LEFT) {
                nf = static_cast<Direction>((cur.facing + 3) % 4);
            } else {
                nf = static_cast<Direction>((cur.facing + 1) % 4);
            }
            if (nx < 0 || nx >= GRID_SIZE || ny < 0 || ny >= GRID_SIZE) continue;
            if (obstacleMap[nx][ny]) continue;
            float nc = cur.cost + costFn(cur.facing, m.command);
            auto key = std::make_tuple(nx, ny, nf);
            if (visited.count(key) && visited[key] <= nc) continue;
            visited[key] = nc;
            Node next{nx, ny, nf, nc, heurFn(nx, ny, targetX, targetY, nf), cur.path};
            next.path.push_back(m);
            openSet.push(next);
        }
    }
    return {};
}

int Algorithm::manhattanDistance(int x1, int y1, int x2, int y2) const {
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

float Algorithm::heuristic(int x1, int y1, int x2, int y2, Direction d) const {
    return manhattanDistance(x1, y1, x2, y2) * MOVE_COST;
}

void Algorithm::updateObstacleMap(int x, int y, bool hasObstacle) {
    obstacleMap[x][y] = hasObstacle;
    if (hasObstacle) {
        currentPath.clear();
    }
}
