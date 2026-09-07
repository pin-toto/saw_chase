#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Collision.h"
#include <ctime>
#include <cmath>
#include "pt_audio.hpp"
int main(int argc, char* argv[]) {
    srand(time(0));
    
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    
    SDL_Window* win= SDL_CreateWindow(
        "Saw Chase",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        640,
        480,
        SDL_WINDOW_SHOWN


    );

    const int FPS = 30;
    const int frameDelay = 1000 / FPS;
    bool run=true;
    SDL_Event ev;
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");
    SDL_Renderer* ren=SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED);

    SDL_Rect bak={0,0,320,240};

    SDL_Rect pl_src ={11,9,167,155};
    SDL_Rect pl_dst ={144,104,32,32};
    double pl_angle =0;

    SDL_Rect coin_src={203,122,70,70};
    SDL_Rect coin_dst={rand() % 320,rand() % 240,16,16};

    SDL_Rect enemy_src={187,3,110,112};
    SDL_Rect enemy_dst={rand() % 320,rand() % 240,22,22};
    float enemy_angle=0;


    

    SDL_Surface* enemy_img=IMG_Load("tmps/imgs.png");
    SDL_Surface* img_bak=IMG_Load("tmps/bak.png");
    SDL_Surface* pl_img=IMG_Load("tmps/imgs.png");
    SDL_Surface* coin_img=IMG_Load("tmps/imgs.png");

    SDL_Texture* bak_tex=SDL_CreateTextureFromSurface(ren,img_bak);
    SDL_Texture* pl_tex=SDL_CreateTextureFromSurface(ren,pl_img);
    SDL_Texture* coin_tex=SDL_CreateTextureFromSurface(ren,coin_img);
    SDL_Texture* enemy_tex=SDL_CreateTextureFromSurface(ren,enemy_img);


    SDL_FreeSurface(img_bak);
    SDL_FreeSurface(pl_img);
    SDL_FreeSurface(coin_img);
    SDL_FreeSurface(enemy_img);
    SDL_RenderSetLogicalSize(ren, 320, 240);


    while(run) {

        Uint32 frameStart = SDL_GetTicks();

        while(SDL_PollEvent(&ev)) {

            if(ev.type == SDL_QUIT) {

                run=false;

            }

        }
        

        SDL_SetRenderDrawColor(ren,0,0,0,255);
        SDL_RenderClear(ren);
        
        const Uint8* kay=SDL_GetKeyboardState(NULL);

        
    
    
        if(kay[SDL_SCANCODE_W]) {

            pl_angle =360;
            pl_dst.y -= 3;

        }else if(kay[SDL_SCANCODE_S]) {

            pl_angle =180;
            pl_dst.y += 3;


        }else if(kay[SDL_SCANCODE_D]) {

            pl_angle=90;
            pl_dst.x += 3;

        }else if(kay[SDL_SCANCODE_A]) {

            pl_angle= -90;
            pl_dst.x -= 3;


        }

        if(pl_dst.x < 10) {

            pl_dst.x=10;

        }else if(pl_dst.x > 280) {

            pl_dst.x = 280;

        }

        if(pl_dst.y < 2) {

            pl_dst.y=2;

        }else if(pl_dst.y > 208) {

            pl_dst.y=208;

        }


        if(checkCollision(pl_dst,coin_dst)) {
            pt_audio::play_tone_ms_async(1800,500);
            coin_dst.x=rand() % 280;
            coin_dst.y=rand() %240;

        }


        if(checkCollision(enemy_dst,pl_dst)) {

            pt_audio::play_tone_ms_async(3500,500);
            pt_audio::play_tone_ms_async(1200,500);

        }

    float dx = pl_dst.x - enemy_dst.x;
    float dy = pl_dst.y - enemy_dst.y;

    float distance = sqrt(dx * dx + dy * dy);

    if (distance > 0) {
     

        enemy_dst.x += dx / distance * 2.0f;
        enemy_dst.y += dy / distance * 2.0f;
    }



        SDL_RenderCopy(ren,bak_tex,NULL,&bak);
        SDL_RenderCopyEx(ren,pl_tex,&pl_src,&pl_dst,pl_angle,NULL,SDL_FLIP_NONE);
        SDL_RenderCopy(ren,coin_tex,&coin_src,&coin_dst);
        enemy_angle+=10;
        if(enemy_angle > 360.0) {

            enemy_angle=0;

        }
        SDL_RenderCopyEx(ren,enemy_tex,&enemy_src,&enemy_dst,enemy_angle,NULL,SDL_FLIP_NONE);

        SDL_RenderPresent(ren);


    //set FPS game
    Uint32 frameTime = SDL_GetTicks() - frameStart;

    if (frameTime < frameDelay) {

        SDL_Delay(frameDelay - frameTime);


    }//end FPS



    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyTexture(bak_tex);
    SDL_DestroyTexture(pl_tex);
    SDL_DestroyTexture(coin_tex);
    SDL_DestroyTexture(enemy_tex);
    SDL_DestroyWindow(win);
    IMG_Quit();
    SDL_Quit();
    return 0;

}
