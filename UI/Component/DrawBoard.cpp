#include <allegro5/allegro_primitives.h>
#include <vector>
#include "UI/Component/DrawBoard.hpp"
#include "Image.hpp"
#include "Engine/LOG.hpp"
#include "ML_Macro.hpp"

#if USE_ML

// #include "ML_Resource/ml_pch.hpp.gch"
#include "ML_Resource/ml_pch.hpp"
// #include <mlpack/base.hpp>
// #include <mlpack/config.hpp>
// #include <mlpack/core.hpp>
// #include <mlpack/methods/ann/ffn.hpp>
#include <armadillo>
using namespace mlpack;
using namespace arma;


#endif

using namespace std;
using namespace Engine;

const int DrawBoard::PixelWidth = 20;
const int DrawBoard::PictureWidth = 28;

static const int dirX[] = {0, -1, 0, -1, 1, 1, 1, 0, -1};
static const int dirY[] = {0, -1, -1, 0, -1, 0, 1, 1, 1};
static const int numOfDirection = 9;

DrawBoard::DrawBoard(int x, int y)
{
    Position.x = x;
    Position.y = y;

    AddNewObject(PixelGroup = new Group());

    // tile init
    pixel_ptr.assign(PictureWidth, vector<Engine::Image*>(PictureWidth, nullptr));
    pixel_status.assign(PictureWidth, vector<PixelType>(PictureWidth, WHITE));
    for(int i = 0; i < PictureWidth; ++i)
    {
        for(int j = 0 ; j < PictureWidth; ++j)
        {
            pixel_ptr[i][j] = new Engine::Image("stage-select/white_tile.png", 
                        x + j * PixelWidth, y + i * PixelWidth, 
                        PixelWidth, PixelWidth);
            PixelGroup->AddNewObject(pixel_ptr[i][j]);
        }
    }
}

void DrawBoard::OnMouseDown(int button, int mx, int my)
{
    if(button == 1)
        mouse_holding = true;
    OnMouseMove(mx, my);
}

void DrawBoard::OnMouseUp(int button, int mx, int my)
{
    if(button == 1)
        mouse_holding = false;
    OnMouseMove(mx, my);
}

void DrawBoard::OnMouseMove(int mx, int my)
{
    if(mouse_holding)
    {
        const int x = (mx - this->Position.x) / PixelWidth;
        const int y = (my - this->Position.y) / PixelWidth;

        for(int i = 0; i < 4; ++i)
        {
            int dx = dirX[i];
            int dy = dirY[i];
            int newX = x + dx;
            int newY = y + dy;

            if(newX < 0 || newX >= PictureWidth || newY < 0 || newY >= PictureWidth)
                continue;
            DrawOnBoard(newX, newY, BLACK);
        
        }

        for(int i = 4; i < 9; ++i)
        {
            int dx = dirX[i];
            int dy = dirY[i];
            int newX = x + dx;
            int newY = y + dy;

            if(newX < 0 || newX >= PictureWidth || newY < 0 || newY >= PictureWidth)
                continue;
            DrawOnBoard(newX, newY, GRAY);
        }
    }
}

void DrawBoard::DrawOnBoard(int x, int y)
{
    if(pixel_status[y][x] == BLACK)
        return;
    pixel_status[y][x] = BLACK;
    PixelGroup->RemoveObject(pixel_ptr[y][x]->GetObjectIterator());
    pixel_ptr[y][x] = new Engine::Image("stage-select/black_tile.png", 
            Position.x + x * PixelWidth, Position.y + y * PixelWidth, 
            PixelWidth, PixelWidth);
    PixelGroup->AddNewObject(pixel_ptr[y][x]);
}

void DrawBoard::DrawOnBoard(int x, int y, PixelType ty)
{
    if(pixel_status[y][x] == BLACK)
        return;
    

    if(ty == BLACK)
    {
        pixel_status[y][x] = BLACK;
        PixelGroup->RemoveObject(pixel_ptr[y][x]->GetObjectIterator());
        pixel_ptr[y][x] = new Engine::Image("stage-select/black_tile.png", 
                Position.x + x * PixelWidth, Position.y + y * PixelWidth, 
                PixelWidth, PixelWidth);
    }
    else if(ty == GRAY)
    {
        pixel_status[y][x] = GRAY;
        PixelGroup->RemoveObject(pixel_ptr[y][x]->GetObjectIterator());
        pixel_ptr[y][x] = new Engine::Image("stage-select/gray_tile.png", 
                Position.x + x * PixelWidth, Position.y + y * PixelWidth, 
                PixelWidth, PixelWidth);
    }
    PixelGroup->AddNewObject(pixel_ptr[y][x]);
}

#if USE_ML
int DrawBoard::GetNumber() const
{
    FFN<NegativeLogLikelihood, RandomInitialization> model;
    const string fileName = GetModelName();

    LOG(INFO) << "Loading CNN model: " << fileName << '\n';
    data::Load(fileName, "CNNmodel", model, true);

    arma::Col<double> input(PictureWidth * PictureWidth);
    for(int i = 0; i < PictureWidth * PictureWidth; ++i)
    {
        PixelType ty = pixel_status[i / PictureWidth][i % PictureWidth];
        if(ty == WHITE)
            input(i) = 0;
        else if(ty == BLACK)
            input(i) = 1;
        else if(ty == GRAY)
            input(i) = 0.8;
    }
    

    arma::colvec result;
    model.Predict(input, result);
    


    LOG(INFO) << "CNN output: " << result.index_max();
    return result.index_max();
}

std::string DrawBoard::GetModelName() const
{
    int version = static_cast<int>(CNN_MODEL_VERSION);
    std::string fileName = "CNN_models/CNNmodel" + to_string(version) + ".bin";
    return fileName;
}

#endif 


void DrawBoard::ClearBoard()
{
    for(int i = 0; i < PictureWidth; ++i)
    {
        for(int j = 0; j < PictureWidth; ++j)
        {
            if(pixel_status[i][j] != WHITE)
            {
                pixel_status[i][j] = WHITE;
                PixelGroup->RemoveObject(pixel_ptr[i][j]->GetObjectIterator());
                pixel_ptr[i][j] = new Engine::Image("stage-select/white_tile.png", 
                        Position.x + j * PixelWidth, Position.y + i * PixelWidth, 
                        PixelWidth, PixelWidth);
                PixelGroup->AddNewObject(pixel_ptr[i][j]);
            }
        }
    }
}

void DrawBoard::SetFileOutput(const std::string &fileName)
{
    fout.open(fileName, std::ios_base::app);
}

void DrawBoard::OutputDataPoint(int label)
{
    string str;
    str = to_string(label);

    for(int i = 0; i < PictureWidth * PictureWidth; ++i)
    {
        PixelType ty = pixel_status[i / PictureWidth][i % PictureWidth];
        if(ty == WHITE)
            str += ",0";
        else if(ty == BLACK)
            str += ",255";
        else if(ty == GRAY)
            str += ",204";
    }
    str += "\n";
    fout << str;
}

