#include "Algorithm.h"
#include "Logger.h"
#include <queue>
#include <tuple>
#include <map>
#include <climits>
#include <cmath>

static const float MOVE_COST     = 2.0f;
static const float ROTATION_COST = 1.5f;

Algorithm::Algorithm(House* h, VacuumCleaner* v)
  : currentObjective(CLEANING)
  , currentPath()
  , house(h)
  , vacuum(v)
{
    // Initialize obstacle map
    for (int i = 0; i < GRID_SIZE; ++i) {
      for (int j = 0; j < GRID_SIZE; ++j) {
        obstacleMap[i][j] = false;
      }
    }
    logger.log("Algorithm initialized (GRID_SIZE=%d)", GRID_SIZE);
}

void Algorithm::setObjective(AlgorithmObjective obj) {
    currentObjective = obj;
    currentPath.clear();
    logger.log("Objective set to %s", obj == CLEANING ? "CLEANING" : "RETURN_HOME");
}

std::list<MovementCommand> Algorithm::getCurrentPath() const {
    return currentPath;
}

void Algorithm::calculateNextMove() {
    if (currentObjective == RETURN_HOME) {
        logger.log("Calculating return-to-dock path");
        calculateReturnPath();
        return;
    }
    if (!currentPath.empty()) return;

    logger.log("Finding highest priority dirty cell...");
    CellPriority target = findHighestPriorityCell();

    if (target.dirtLevel == 0) {
        logger.log("No dirt found, switching to RETURN_HOME");
        currentObjective = RETURN_HOME;
        calculateReturnPath();
    } else {
        logger.log("Target dirty cell at (%d,%d) level=%d", target.x, target.y, target.dirtLevel);
        currentPath = aStarPathfinding(target.x, target.y);
        logger.log("Planned path of %d steps", (int)currentPath.size());
    }
}

CellPriority Algorithm::findHighestPriorityCell() const {
    CellPriority best{0,0,0,-INFINITY};
    int rx = vacuum->getX(), ry = vacuum->getY();

    for (int x = 0; x < GRID_SIZE; ++x) {
      for (int y = 0; y < GRID_SIZE; ++y) {
        if (obstacleMap[x][y]) continue;
        int d = house->getDirtLevel(x, y);
        float score = d * 100.0f - manhattanDistance(rx, ry, x, y);
        if (score > best.priorityScore) {
          best = {x, y, d, score};
        }
      }
    }
    logger.log("Best cell: (%d,%d) dirt=%d score=%.1f", best.x, best.y, best.dirtLevel, best.priorityScore);
    return best;
}

void Algorithm::calculateReturnPath() {
    auto [homeX, homeY] = house->getDockingStation();
    logger.log("Building path home to dock at (%d,%d)", homeX, homeY);
    currentPath = aStarPathfinding(homeX, homeY);
    logger.log("Return path length: %d", (int)currentPath.size());
}

std::list<MovementCommand> Algorithm::aStarPathfinding(int tx, int ty) {
    struct Node {
      int x, y;
      Direction facing;
      float cost;
      float heuristic;
      std::list<MovementCommand> path;
      bool operator<(Node const& o) const {
        return cost + heuristic > o.cost + o.heuristic;
      }
    };

    auto heurFn = [&](int x1,int y1,int x2,int y2,Direction d) {
      return manhattanDistance(x1,y1,x2,y2) * MOVE_COST;
    };
    auto costFn = [&](Direction f, Command c) {
      return (c == FORWARD) ? MOVE_COST : ROTATION_COST;
    };

    std::priority_queue<Node> openSet;
    std::map<std::tuple<int,int,Direction>,float> visited;

    Node start{
      vacuum->getX(), vacuum->getY(), vacuum->getDirection(),
      0.0f,
      heurFn(vacuum->getX(), vacuum->getY(), tx, ty, vacuum->getDirection()),
      {}
    };
    openSet.push(start);
    visited[{start.x,start.y,start.facing}] = 0.0f;

    while (!openSet.empty()) {
      Node cur = openSet.top(); openSet.pop();
      if (cur.x == tx && cur.y == ty) {
        return cur.path;
      }

      for (int cmd = FORWARD; cmd <= RIGHT; ++cmd) {
        MovementCommand m{static_cast<Command>(cmd)};
        int nx = cur.x, ny = cur.y;
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

        float newCost = cur.cost + costFn(cur.facing, m.command);
        auto key = std::make_tuple(nx, ny, nf);
        if (visited.count(key) && visited[key] <= newCost) continue;
        visited[key] = newCost;

        Node next{
          nx, ny, nf,
          newCost,
          heurFn(nx, ny, tx, ty, nf),
          cur.path
        };
        next.path.push_back(m);
        openSet.push(next);
      }
    }

    logger.log("No path found to (%d,%d)", tx, ty);
    return {};
}

int Algorithm::manhattanDistance(int x1,int y1,int x2,int y2) const {
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}

float Algorithm::heuristic(int x1,int y1,int x2,int y2,Direction d) const {
    return manhattanDistance(x1,y1,x2,y2) * MOVE_COST;
}

void Algorithm::updateObstacleMap(int x, int y, bool hasObs) {
    obstacleMap[x][y] = hasObs;
    if (hasObs) {
        currentPath.clear();
        logger.log("Obstacle at (%d,%d), clearing path", x, y);
    }
}
