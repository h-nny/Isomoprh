#include <iostream>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_main.h>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int VERTEX_SIZE = 10;

struct Point {
    int x;
    int y;

    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Point& other) const {
        return !(*this == other);
    }
};

void drawVertices(SDL_Renderer* renderer, const std::vector<Point>& points) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

    for (const auto& point : points) {
        SDL_Rect rect;
        rect.x = point.x - VERTEX_SIZE / 2;
        rect.y = point.y - VERTEX_SIZE / 2;
        rect.w = VERTEX_SIZE;
        rect.h = VERTEX_SIZE;
        SDL_RenderFillRect(renderer, &rect);
    }
}

void drawEdges(SDL_Renderer* renderer, const std::vector<Point>& points) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

    if (points.size() >= 2) {
        SDL_RenderDrawLines(renderer, reinterpret_cast<const SDL_Point*>(points.data()), points.size());
    }
}

bool isDuplicateEdge(const Point& startPoint, const Point& endPoint, const std::vector<Point>& edges) {
    for (size_t i = 0; i < edges.size(); i += 2) {
        if ((edges[i] == startPoint && edges[i + 1] == endPoint) ||
            (edges[i] == endPoint && edges[i + 1] == startPoint)) {
            return true;
        }
    }
    return false;
}

int SDL_main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Polygon Drawer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (window == nullptr) {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if (renderer == nullptr) {
        std::cout << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    int vertices, edges;
    std::cout << "Enter the number of vertices: ";
    std::cin >> vertices;
    std::cout << "Enter the number of edges: ";
    std::cin >> edges;

    std::vector<Point> verticesPoints;
    std::vector<Point> edgesPoints;
    bool vertexToolActive = false;
    bool edgeToolActive = false;
    Point edgeStartPoint;
    bool edgeStartPointSet = false;
    int edgeCount = 0;
    bool quit = false;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (vertexToolActive) {
                        Point point;
                        point.x = event.motion.x;
                        point.y = event.motion.y;
                        verticesPoints.push_back(point);
                        edgeStartPointSet = false;
                    } else if (edgeToolActive) {
                        if (verticesPoints.size() < vertices) {
                            std::cout << "Please add vertices before creating edges." << std::endl;
                        } else {
                            if (!edgeStartPointSet) {
                                for (size_t i = 0; i < verticesPoints.size(); ++i) {
                                    const Point& vertex = verticesPoints[i];
                                    if (event.motion.x >= vertex.x - VERTEX_SIZE / 2 &&
                                        event.motion.x <= vertex.x + VERTEX_SIZE / 2 &&
                                        event.motion.y >= vertex.y - VERTEX_SIZE / 2 &&
                                        event.motion.y <= vertex.y + VERTEX_SIZE / 2) {
                                        edgeStartPoint = vertex;
                                        edgeStartPointSet = true;
                                        break;
                                    }
                                }
                            } else {
                                bool validEndpoint = false;
                                for (size_t i = 0; i < verticesPoints.size(); ++i) {
                                    const Point& vertex = verticesPoints[i];
                                    if (event.motion.x >= vertex.x - VERTEX_SIZE / 2 &&
                                        event.motion.x <= vertex.x + VERTEX_SIZE / 2 &&
                                        event.motion.y >= vertex.y - VERTEX_SIZE / 2 &&
                                        event.motion.y <= vertex.y + VERTEX_SIZE / 2) {
                                        if (i != verticesPoints.size() - 1 && vertex != edgeStartPoint &&
                                            !isDuplicateEdge(edgeStartPoint, vertex, edgesPoints)) {
                                            Point point;
                                            point.x = vertex.x;
                                            point.y = vertex.y;
                                            edgesPoints.push_back(edgeStartPoint);
                                            edgesPoints.push_back(point);
                                            edgeStartPointSet = false;
                                            validEndpoint = true;
                                            edgeCount++;
                                        }
                                        break;
                                    }
                                }
                                if (!validEndpoint) {
                                    edgeStartPointSet = false;
                                }
                            }
                        }

                        if (edgeCount >= edges) {
                            std::cout << "Maximum number of edges reached." << std::endl;
                            edgeToolActive = false;
                            edgeStartPointSet = false;
                        }
                    }
                }
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_f) {
                    edgeToolActive = true;
                    vertexToolActive = false;
                    std::cout << "Edge tool selected." << std::endl;
                } else if (event.key.keysym.sym == SDLK_g) {
                    vertexToolActive = true;
                    edgeToolActive = false;
                    std::cout << "Vertex tool selected." << std::endl;
                    edgeStartPointSet = false;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderClear(renderer);

        drawEdges(renderer, edgesPoints);
        drawVertices(renderer, verticesPoints);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
