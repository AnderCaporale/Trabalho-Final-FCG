#include "collisions.hpp"
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

#define WALLMARGIN_2 0.1f

std::vector<Wall> getClosestWalls(glm::vec4& player_position, std::vector<Wall>& walls)
{
    std::vector<Wall> closest_walls = std::vector<Wall>();

    for (auto& wall : walls){
        if (fabs(player_position.x - wall.position.x) <= 1.0f && fabs(player_position.z - wall.position.z) <= 1.0f){
            closest_walls.push_back(wall);
        }
    }

    return closest_walls;
}

std::tuple<glm::vec4, glm::vec4> getFacePointAndNormal(Wall& wall, FaceDirection face, glm::vec4& player_position){
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
    if(player_position.x < c.x)
        c.x -= WALLMARGIN_2;
    else
        c.x += WALLMARGIN_2;
    if(player_position.y < c.y)
        c.y -= WALLMARGIN_2;
    else
        c.y += WALLMARGIN_2;
    if(player_position.z < c.z)
        c.z -= WALLMARGIN_2;
    else
        c.z += WALLMARGIN_2;

    return std::make_tuple(c, n);
}

std::tuple<bool, Wall, FaceDirection, glm::vec4, glm::vec4> getCollisionWallAndFace(glm::vec4& player_position, glm::vec4& move_direction, std::vector<Wall>& walls)
{
    bool has_collision = false;
    Wall closest_wall;
    closest_wall.position.x = -100.0f;
    closest_wall.position.z = -100.0f;
    float t = 1.0f;
    FaceDirection collision_face;
    glm::vec4 collision_point;
    glm::vec4 c;
    glm::vec4 n;
    glm::vec4 collision_normal;

    for (auto& wall : walls){
        for(int face = FaceDirection::NORTH; face <= FaceDirection::BOTTOM; face++){
            std::tie(c, n) = getFacePointAndNormal(wall, FaceDirection(face), player_position);
            float collision_coef = glm::dot(c - player_position, n) / glm::dot(move_direction, n);
            if(collision_coef < t && collision_coef > 0.0f){
                glm::vec4 tmp_collision_point = player_position + collision_coef * move_direction;
                bool test = false;
                switch(wall.dir){
                    case XYWALL:
                        test = tmp_collision_point.x >= c.x - WALLMARGIN_2 &&
                               tmp_collision_point.x <= c.x + 1.0f + WALLMARGIN_2 &&
                               tmp_collision_point.y >= c.y - WALLMARGIN_2 &&
                               tmp_collision_point.y <= c.y + 1.0f + WALLMARGIN_2 &&
                               tmp_collision_point.z >= c.z - WALLMARGIN_2 &&
                               tmp_collision_point.z <= c.z + 0.01f + WALLMARGIN_2;
                        break;
                    case YZWALL:
                        test = tmp_collision_point.x >= c.x - WALLMARGIN_2 &&
                               tmp_collision_point.x <= c.x + 0.01f + WALLMARGIN_2 &&
                               tmp_collision_point.y >= c.y - WALLMARGIN_2 &&
                               tmp_collision_point.y <= c.y + 1.0f + WALLMARGIN_2 &&
                               tmp_collision_point.z >= c.z - WALLMARGIN_2 &&
                               tmp_collision_point.z <= c.z + 1.0f + WALLMARGIN_2;
                        break;
                }
                if (test){
                    t = collision_coef;
                    closest_wall = wall;
                    collision_face = FaceDirection(face);
                    has_collision = true;
                    collision_point = tmp_collision_point;
                    collision_normal = n;
                }
            }
        }
    }
    if(t == 1.0f)
        has_collision = false;
    return std::make_tuple(has_collision, closest_wall, collision_face, collision_point, collision_normal);
}

void checkCollisionWithWalls(glm::vec4& player_position, glm::vec4& move_direction, std::vector<Wall>& walls)
{
    std::vector<Wall> closest_walls = getClosestWalls(player_position, walls);

    bool has_collision;
    Wall collision_wall;
    FaceDirection collision_face;
    glm::vec4 collision_point;
    glm::vec4 n;

    std::tie(has_collision, collision_wall, collision_face, collision_point, n) = getCollisionWallAndFace(player_position, move_direction, closest_walls);
    if(has_collision){
        move_direction = (move_direction - n*dot(move_direction, n))*0.5f;
    }

    if(player_position.x < -13.0f)
        player_position.x = -13.0f;
    if(player_position.x > 13.0f)
        player_position.x = 13.0f;
    if(player_position.z > 1.0f)
        player_position.z = 1.0f;
    if(player_position.z < -25.0f)
        player_position.z = -25.0f;

}

bool checkCollisionWithBunnies(glm::vec4 &player_position, std::vector<Bunny> &bunnies, int &score)
{
    for (int i = 0; i < bunnies.size(); i++)
    {
        //Check if player is inside a circle of radius 0.3f around the bunny
        if (pow(player_position.x - bunnies[i].position.x,2) +
            pow(player_position.y - bunnies[i].position.y,2) +
            pow(player_position.z - bunnies[i].position.z,2) <= 0.09f)
        {
            bunnies.erase(bunnies.begin() + i);
            score++;
            i--;
            return true;
        }
    }
    return false;
}
