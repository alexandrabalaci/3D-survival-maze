#pragma once

#include "components/simple_scene.h"
#include "lab_m1/tema2/hcamera.h"


namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
    public:
        Tema2();
        ~Tema2();

        void Init() override;
        struct ViewportSpace
        {
            ViewportSpace() : x(0), y(0), width(1), height(1) {}
            ViewportSpace(int x, int y, int width, int height)
                : x(x), y(y), width(width), height(height) {}
            int x;
            int y;
            int width;
            int height;
        };

        struct LogicSpace
        {
            LogicSpace() : x(0), y(0), width(1), height(1) {}
            LogicSpace(float x, float y, float width, float height)
                : x(x), y(y), width(width), height(height) {}
            float x;
            float y;
            float width;
            float height;
        };

        struct Entity
        {
            GLfloat scaleX;
            GLfloat scaleY;
            GLfloat scaleZ;
            GLfloat posY;
            GLfloat posX;
            GLfloat posZ;
            GLfloat sizeX;
            GLfloat sizeY;
            GLfloat sizeZ;
            GLfloat angle;
            GLfloat health;
            GLfloat start;
            GLfloat direction;
            GLfloat distance;
            int render;
            GLfloat speed;
            glm::vec3 directionVector;
            glm::vec3 pos;
        };


    private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) override;

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, const glm::vec3& color);
        void RenderPlayer();
        void RenderMaze();
        void RenderEnemies(GLfloat deltaTimeSeconds);
        void DoPlayerMazeCollisions();
        void RenderProjectile(GLfloat deltaTimeSeconds);
        GLboolean CheckCollision(const Entity& one, int positionX, int positionY, const Entity& two);
        void RenderAnimationMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, float animation_time);
        glm::mat3 VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);
        void SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor = glm::vec3(0), bool clear = true);


    protected:
        m1::Camera* camera;
        glm::mat4 projectionMatrix;
        int projectilesNo = 0;

        GLfloat right = 10;
        GLfloat left = 0;
        GLfloat bottom = 0;
        GLfloat top = 10;
        GLfloat fov = 60;
        GLfloat zNear = 0.01;
        GLfloat zFar = 200;

        glm::mat4 modelMatrix, playerMatrix, enemyMatrix, projectileMatrix;
        glm::mat3 visMatrix, modelMatrix2D;
        float scaleX, scaleY, scaleZ;

        Entity player, healthBar, healthBarEmpty, timeBar, timeBarEmpty;

        bool firstPerson = false;
        std::vector<glm::vec3> maze;
        std::vector<std::vector<int>> maze_matrix;

        std::vector<Entity> enemies;
        int enemiesNo = 0;
        int mazeBlocksNo = 0;
        std::vector<Entity> mazeBlocks;


        GLfloat timePassedProjectile = 0;
        GLfloat timeLeft = 120;
        GLboolean wasMousePressed = false;

        std::vector<Entity> projectiles;
        std::vector<Entity> healthBonuses;
        std::vector<Entity> scoreBonuses;
        ViewportSpace viewSpace;
        LogicSpace logicSpace;

    };
}   // namespace m1
