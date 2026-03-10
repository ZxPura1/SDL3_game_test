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
	//Symbolic constant for original image size
	static constexpr float kOriginalSize{ -1.f };
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
			//color key image
			if (SDL_SetSurfaceColorKey(loadedSurface, true, SDL_MapSurfaceRGB(loadedSurface, 0xFF, 0xFF, 0xFF)) == false)
			{
				SDL_Log("Unable to set the color key! SDL error: %s\n", SDL_GetError());
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
	void render(float x, float y, SDL_FRect* clip = nullptr, float width = kOriginalSize, float height = kOriginalSize)
	{
		//Set texture position
		SDL_FRect dstRect{ x,y, static_cast<float>(mWidth), static_cast<float>(mHeight) };

		//Default to clip dimensions if clip is given
		if (clip != nullptr)
		{
			dstRect.w = clip->w;
			dstRect.h = clip->h;
		}

		//Resize if new dimensions are given
		if (width > 0)
		{
			dstRect.w = width;
		}
		if (height > 0)
		{
			dstRect.h = height;
		}

		//Render texture
		SDL_RenderTexture(gRenderer, mTexture, clip, &dstRect);
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

//The directional images
LTexture gUpTexture, gDownTexture, gLeftTexture, gRightTexture;

//The sprite sheet texture
LTexture gspriteSheet;


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

	if (gspriteSheet.loadFromFile("assets/dots.png") == false)
	{
		SDL_Log("Unable to load sprite sheet image!\n");
		success = false;
	}
	
	if (gUpTexture.loadFromFile("assets/up.png") == false)
	{
		SDL_Log("Unable to load up image!\n");
		success = false;
	}
	if (gDownTexture.loadFromFile("assets/down.png") == false)
	{
		SDL_Log("Unable to load down image!\n");
		success = false;
	}
	if (gLeftTexture.loadFromFile("assets/left.png") == false)	
		{
		SDL_Log("Unable to load left image!\n");
		success = false;
	}
	if (gRightTexture.loadFromFile("assets/right.png") == false)
	{
		SDL_Log("Unable to load right image!\n");
		success = false;
	}

	return success;
}

void close()
{
	//Clean up the surface
	gspriteSheet.destroy();

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

			//The currently rendered texture
			LTexture* currentTexture{ &gUpTexture };
			//LTexture* gSpriteSheetTexture{ &gspriteSheet };

			//Background color defaults to white
			SDL_Color bgColor{ 0xFF, 0xFF, 0xFF, 0xFF };

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
					else if (e.type == SDL_EVENT_KEY_DOWN)
					{
						//Set texture 
						if (e.key.key == SDLK_W)
						{
							currentTexture = &gUpTexture;
						}
						else if (e.key.key == SDLK_S)
						{
							currentTexture = &gDownTexture;
						}
						else if (e.key.key == SDLK_A)
						{
							currentTexture = &gLeftTexture;
						}
						else if (e.key.key == SDLK_D)
						{
							currentTexture = &gRightTexture;
						}
					}
				}
				// reset background color
				bgColor.r = 0xFF;
				bgColor.g = 0xFF;
				bgColor.b = 0xFF;

				//Set background color based on key state
				const bool* keyStates = SDL_GetKeyboardState(nullptr);
				if (keyStates[SDL_SCANCODE_W] == true)
				{
					//red
					bgColor.r = 0xFF;
					bgColor.g = 0x00;
					bgColor.b = 0x00;
				}
				else if (keyStates[SDL_SCANCODE_S] == true)
				{
					//Green
					bgColor.r = 0x00;
					bgColor.g = 0xFF;
					bgColor.b = 0x00;
				}
				else if (keyStates[SDL_SCANCODE_A] == true)
				{
					//Yellow
					bgColor.r = 0xFF;
					bgColor.g = 0xFF;
					bgColor.b = 0x00;
				}
				else if (keyStates[SDL_SCANCODE_D] == true)
				{
					//Blue
					bgColor.r = 0x00;
					bgColor.g = 0x00;
					bgColor.b = 0xFF;
				}
				else if (keyStates[SDL_SCANCODE_T] == true)
				{
					//
					bgColor.r = 0x00;
					bgColor.g = 0x00;
					bgColor.b = 0x00;
				}

				//Fill the background
				SDL_SetRenderDrawColor(gRenderer, bgColor.r, bgColor.g, bgColor.b, 0xFF);
				SDL_RenderClear(gRenderer);

				//Init sprite clip
				constexpr float kSpriteSize = 100.f;
				SDL_FRect spriteClip{ 0.f, 0.f, kSpriteSize, kSpriteSize };

				//Init sprite size
				SDL_FRect spriteSize{ 0.f, 0.f, kSpriteSize, kSpriteSize };

				//use top left sprite
				spriteClip.x = 0.f;
				spriteClip.y = 0.f;

				//Set sprite size to original size
				spriteSize.w = kSpriteSize * 3.0f;
				spriteSize.h = kSpriteSize * 3.0f;

				//draw original sized sprite
				gspriteSheet.render(kScreenWidth * 0.5f - spriteSize.w * 0.5f, kScreenHeight * 0.5f - spriteSize.h * 0.5f, &spriteClip, spriteSize.w, spriteSize.h);

				//use top right sprite
				spriteClip.x = kSpriteSize;
				spriteClip.y = 0.f;

				//set sprite size to half size
				spriteSize.w = kSpriteSize * 0.5f;
				spriteSize.h = kSpriteSize * 0.5f;

				//draw half sized sprite
				gspriteSheet.render(kScreenWidth - spriteSize.w, 0.f, &spriteClip, spriteSize.w, spriteSize.h);

				//Render image on screen
				//currentTexture->render((kScreenWidth - currentTexture->getWidth()) / 2.f, (kScreenHeight - currentTexture->getHeight()) / 2.f);

				//Update screen
				SDL_RenderPresent(gRenderer);

			}
			//Clean up (exit the loop)
			close();
			return exitCode;
		}
	}
}