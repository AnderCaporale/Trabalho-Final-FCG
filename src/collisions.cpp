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

#define XYWALL      8
#define YZWALL      9

std::vector<Wall> getClosestWalls(glm::vec4& player_position, std::vector<Wall>& walls)
{
    std::vector<Wall> closest_walls;

    for (auto& wall : walls){
        if (fabs(player_position.x - wall.position.x) <= 1.0f && fabs(player_position.z - wall.position.z) <= 1.0f){
            closest_walls.push_back(wall);
        }
    }
    // std::cout << "Player position: " << player_position.x << " " << player_position.z << "  |  ";
    // std::cout << "Closest walls: " << closest_walls.size() << std::endl;

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

std::tuple<bool, Wall, FaceDirection, glm::vec4> getCollisionWallAndFace(glm::vec4& player_position, glm::vec4& move_direction, std::vector<Wall>& walls)
{
    std::vector<std::tuple<Wall, FaceDirection>> collision_walls;
    bool has_collision = false;
    Wall closest_wall;
    closest_wall.position.x = -100.0f;
    closest_wall.position.z = -100.0f;
    float t = 1.00001f;
    FaceDirection collision_face;
    glm::vec4 collision_point;

    for (auto& wall : walls){
        for(int face = FaceDirection::NORTH; face <= FaceDirection::BOTTOM; face++){
            glm::vec4 c;
            glm::vec4 n;
            std::tie(c, n) = getFacePointAndNormal(wall, FaceDirection(face));
            float collision_coef = glm::dot(c - player_position, n) / glm::dot(move_direction, n);
            if(collision_coef < t && collision_coef > 0.0f){
                glm::vec4 tmp_collision_point = player_position + collision_coef * move_direction;
                switch(wall.dir){
                    case XYWALL:
                        if(tmp_collision_point.x - wall.position.x < 1.0f &&
                           tmp_collision_point.y - wall.position.y < 1.0f &&
                           tmp_collision_point.z - wall.position.z < 0.01f){
                            collision_walls.push_back(std::make_tuple(wall, FaceDirection(face)));
                            t = collision_coef;
                            closest_wall = wall;
                            collision_face = FaceDirection(face);
                            has_collision = true;
                            collision_point = tmp_collision_point;
                        }
                        break;
                    case YZWALL:
                        if(tmp_collision_point.x - wall.position.x < 0.01f &&
                           tmp_collision_point.y - wall.position.y < 1.0f &&
                           tmp_collision_point.z - wall.position.z < 1.0f){
                            collision_walls.push_back(std::make_tuple(wall, FaceDirection(face)));
                            t = collision_coef;
                            closest_wall = wall;
                            collision_face = FaceDirection(face);
                            has_collision = true;
                            collision_point = tmp_collision_point;
                        }
                        break;
                }
            }
        }
    }
    return std::make_tuple(has_collision, closest_wall, collision_face, collision_point);
}

void checkCollisionWithWalls(glm::vec4& player_position, glm::vec4& move_direction, std::vector<Wall>& walls)
{
    std::vector<Wall> closest_walls = getClosestWalls(player_position, walls);

    bool has_collision;
    Wall collision_wall;
    FaceDirection collision_face;
    glm::vec4 collision_point;

    std::tie(has_collision, collision_wall, collision_face, collision_point) = getCollisionWallAndFace(player_position, move_direction, closest_walls);
    if(has_collision){
        std::cout << "Collision with wall" << std::endl;
        std::cout << "Collision point: " << collision_point.x << " " << collision_point.y << " " << collision_point.z << std::endl;
        std::cout << "Collision wall: " << collision_wall.position.x << " " << collision_wall.position.y << " " << collision_wall.position.z << std::endl;
        std::cout << "Collision face: " << collision_face << std::endl;
        std::cout << "Move direction: " << move_direction.x << " " << move_direction.y << " " << move_direction.z << std::endl;
        move_direction = (collision_point-player_position)*0.9f;
    }

}