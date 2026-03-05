#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
//#include <iostream>
#include <string>

/* Constants */
//Screen dimesino constant
constexpr int kScreenWidth{ 800 };
constexpr int kScreenHeight{ 600 };

/* Global Variables */
//The window we'll be rendering to
SDL_Window* gWind{ nullptr };

//The renderer used to draw to the window
SDL_Renderer* gRenderer{ nullptr };

/* Class Prototypes*/
class LTexture
{
public:
	//Initializes texture variables
	/*
	we set the values in the member initializer list as opposed to inside of the constructor itself. 
	This is considered a good habit as it allows for better compiler optimization.
	*/
	LTexture():
		mTexture{ nullptr },
		mWidth{ 0 },
		mHeight{ 0 }
	{ }
	

	//Cleans up the texture variables
	~LTexture()
	{
		destroy();
	}

	//Loads texture from disk
	bool loadFromFile(std::string path)
	{
		//Clean up texture if it already exists
		destroy();

		//Load surface
		if (SDL_Surface* loadedSurface = IMG_Load(path.c_str()); loadedSurface == nullptr)
		{
			SDL_Log("Unable to load image %s! SDL_image error: %s\n", path.c_str(), SDL_GetError());
		}
		else
		{
			//Create texture from surface
			if (mTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface); mTexture == nullptr)
			{
				SDL_Log("Unable to create texture from loaded pixels! SDL error: %s\n", SDL_GetError());
			}
			else
			{
				//get image dimensions
				mWidth = loadedSurface->w;
				mHeight = loadedSurface->h;
			}
			
			//Clean up loaded surface
			SDL_DestroySurface(loadedSurface);
		}

		//return success if texture loaded
		return mTexture != nullptr;

	}

	//Cleans up texture
	void destroy()
	{
		SDL_DestroyTexture(mTexture);
		mTexture = nullptr;
		mWidth = 0;
		mHeight = 0;
	}

	//Draws texture
	void render(float x, float y)
	{
		//Set texture position
		SDL_FRect dstRect{ x,y, static_cast<float>(mWidth), static_cast<float>(mHeight) };

		//Render texture
		SDL_RenderTexture(gRenderer, mTexture, nullptr, &dstRect);
	}

	//gets texture attributes
	int getWidth()
	{
		return mWidth;
	}
	int getHeight()
	{
		return mHeight;
	}
	bool isLoaded()
	{
		return mTexture != nullptr;
	}


private:
	//contains texture data
	SDL_Texture* mTexture;

	//Texture dimensions
	int mWidth;
	int mHeight;

};

//The PNG image we will rendering
LTexture gPngTexture;

/* Function Prototypes */
////Starts up SDL and creates window
//bool init();
//
////Loads media
//bool loadMedia();
//
////Frees media and shuts down SDL
//void close();

////The surface contained by the window
//SDL_Surface* gScreenSurface(nullptr);
//
////The image we will show onto the screen
//SDL_Surface* gHelloWorld{ nullptr };

/* Function Implementations */
bool init()
{
	//Initialization flag
	bool success{ true };

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) == false)
	{
		SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Create Window
		//if( initialization ; condition check)
		//if (gWind = SDL_CreateWindow("SDL 3 tutorial hehe", kScreenWidth, kScreenHeight, 0); gWind == nullptr)
		//{
		//	SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
		//	success = false;
		//}
		//else {
		//	//get window surface
		//	gScreenSurface = SDL_GetWindowSurface(gWind);
		//}

		//Create window with renderer
		if (SDL_CreateWindowAndRenderer("SDL3 Tutorial: Textures and EXtension Libraries", kScreenWidth, kScreenHeight, 0, &gWind, &gRenderer) == false)
		{
			SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
		}
	}
	return success;
}

bool loadMedia()
{
	//file loading flag 
	bool success{ true };

	//load splash image
	/*td::string imagePath{ "assets/mona.bmp" };
	if (gHelloWorld = SDL_LoadBMP(imagePath.c_str()); gHelloWorld == nullptr)
	{
		SDL_Log("Unable to load image %s! SDL Error: %s\n", imagePath.c_str(), SDL_GetError());
		success = false; 
		
		The-shape-of-Katokkon-pepper.png

	}*/

	//load splash image
	if (gPngTexture.loadFromFile("assets/mona.bmp") == false)
	{
		SDL_Log("Unable to load png image!\n");
		success = false;
	}

	return success;
}

void close()
{
	//Clean up the surface

	//SDL_DestroySurface(gHelloWorld);
	//gHelloWorld = nullptr;
	gPngTexture.destroy();

	//Destroy window
	SDL_DestroyRenderer(gRenderer);
	gRenderer = nullptr;
	SDL_DestroyWindow(gWind);
	gWind = nullptr;
	//gScreenSurface = nullptr;

	//Quit SDL subsystems
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Final exit code
	int exitCode{ 0 };

	//initialize
	if (init() == false)
	{
		SDL_Log("Unable to initialize program!\n");
		exitCode = 1;

	}
	else
	{
		//Load media
		if (loadMedia() == false)
		{
			SDL_Log("Unable to load media!\n");
			exitCode = 2;
		}
		else
		{
			//The quit flag
			bool quit{ false };

			//The event data
			SDL_Event e;
			SDL_zero(e);

			//The main loop 
			while (quit == false)
			{
				//Get event data
				while (SDL_PollEvent(&e) == true)
				{
					//If event is quit type
					if (e.type == SDL_EVENT_QUIT)
					{
						//end the main loop
						quit = true;
					}

					//fill the surface white
					//SDL_FillSurfaceRect(gScreenSurface, nullptr, SDL_MapSurfaceRGB(gScreenSurface, 0xFF, 0xFF, 0xFF));
					
					//fill the background white
					SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
					SDL_RenderClear(gRenderer);

					//render image on screen 
					//SDL_BlitSurface(gHelloWorld, nullptr, gScreenSurface, nullptr);
					gPngTexture.render(0.f, 0.f);

					//update the surface
					//SDL_UpdateWindowSurface(gWind);
					
					//update the screen
					SDL_RenderPresent(gRenderer);
				}
			}
			//Clean up (exit the loop)
			close();
			return exitCode;
		}
	}
}