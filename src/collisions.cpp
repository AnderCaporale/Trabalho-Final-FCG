#include "structs.hpp"

enum FaceDirection
{
    NORTH,
    EAST,
    SOUTH,
    WEST,
    TOP,
    BOTTOM
};

#define XYWALL      1
#define YZWALL      2

std::vector<Wall> getClosestWalls(glm::vec4& player_position, std::vector<Wall>& walls)
{
    std::vector<Wall> closest_walls;

    for (auto& wall : walls){
        if (fabs(player_position.x - wall.position.x) <= 1.0f && fabs(player_position.z - wall.position.z) <= 1.0f){
            closest_walls.push_back(wall);
        }
    }
    std::cout << "Player position: " << player_position.x << " " << player_position.z << "  |  ";
    std::cout << "Closest walls: " << closest_walls.size() << std::endl;

    return closest_walls;
}

std::tuple<glm::vec4, glm::vec4> getFacePointAndNormal(Wall& wall, FaceDirection face){
    glm::vec4 c;
    glm::vec4 n;
    if(wall.dir == XYWALL){
        switch(face){
            case FaceDirection::NORTH:
                c = glm::vec4(wall.position.x, wall.position.y, wall.position.z + 0.01, 1.0f);
                n = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
                break;
            case FaceDirection::EAST:
                c = glm::vec4(wall.position.x, wall.position.y, wall.position.z, 1.0f);
                n = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
                break;
            case FaceDirection::SOUTH:
                c = glm::vec4(wall.position.x, wall.position.y, wall.position.z, 1.0f);
                n = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
                break;
            case FaceDirection::WEST:
                c = glm::vec4(wall.position.x + 1.0, wall.position.y, wall.position.z, 1.0f);
                n = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
                break;
            case FaceDirection::TOP:
                c = glm::vec4(wall.position.x, wall.position.y + 1.0, wall.position.z, 1.0f);
                n = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
                break;
            case FaceDirection::BOTTOM:
                c = glm::vec4(wall.position.x, wall.position.y, wall.position.z, 1.0f);
                n = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
                break;
        }
    }else if(wall.dir == YZWALL){
        switch(face){
            case FaceDirection::NORTH:
                c = glm::vec4(wall.position.x, wall.position.y, wall.position.z + 1.0, 1.0f);
                n = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
                break;
            case FaceDirection::EAST:
                c = glm::vec4(wall.position.x, wall.position.y, wall.position.z, 1.0f);
                n = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
                break;
            case FaceDirection::SOUTH:
                c = glm::vec4(wall.position.x, wall.position.y, wall.position.z, 1.0f);
                n = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
                break;
            case FaceDirection::WEST:
                c = glm::vec4(wall.position.x + 0.01, wall.position.y, wall.position.z, 1.0f);
                n = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
                break;
            case FaceDirection::TOP:
                c = glm::vec4(wall.position.x, wall.position.y + 1.0, wall.position.z, 1.0f);
                n = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
                break;
            case FaceDirection::BOTTOM:
                c = glm::vec4(wall.position.x, wall.position.y, wall.position.z, 1.0f);
                n = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
                break;
        }
    }
    return std::make_tuple(c, n);
}

std::vector<std::tuple<Wall, FaceDirection>> getCollisionWallAndFace(glm::vec4& player_position, glm::vec4& move_direction, std::vector<Wall>& walls)
{
    std::vector<std::tuple<Wall, FaceDirection>> collision_walls;
    float t = 1.00001f;

    for (auto& wall : walls){
        for(int face = FaceDirection::NORTH; face <= FaceDirection::BOTTOM; face++){
            glm::vec4 c;
            glm::vec4 n;
            std::tie(c, n) = getFacePointAndNormal(wall, FaceDirection(face));
            float collision_coef = glm::dot(c - player_position, n) / glm::dot(move_direction, n);
            if(collision_coef < t && collision_coef > 0.0f){
                glm::vec4 collision_point = player_position + collision_coef * move_direction;
                switch(wall.dir){
                    case XYWALL:
                        if(collision_point.x >= wall.position.x && collision_point.x <= wall.position.x + 1.0f &&
                           collision_point.y >= wall.position.y && collision_point.y <= wall.position.y + 1.0f &&
                           collision_point.z >= wall.position.z && collision_point.z <= wall.position.z + 0.01f){
                            collision_walls.push_back(std::make_tuple(wall, FaceDirection(face)));
                            t = collision_coef;
                        }
                        break;
                    case YZWALL:
                        if(collision_point.y >= wall.position.y && collision_point.y <= wall.position.y + 1.0f &&
                           collision_point.z >= wall.position.z && collision_point.z <= wall.position.z + 1.0f){
                            collision_walls.push_back(std::make_tuple(wall, FaceDirection(face)));
                            t = collision_coef;
                        }
                        break;
                }
            }
        }
    }
}

void checkCollisionWithWalls(glm::vec4& player_position, glm::vec4& move_direction, std::vector<Wall>& walls)
{
    std::vector<Wall> closest_walls = getClosestWalls(player_position, walls);

    //getCollisionWallAndFace(player_position, move_direction, closest_walls);


}