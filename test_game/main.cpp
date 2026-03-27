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
	//symbolic constant
	static constexpr float kOriginalSize = -1.f;

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

	//set color modulation
	void setColor(Uint8 r, Uint8 g, Uint8 b)
	{
		SDL_SetTextureColorMod(mTexture, r, g, b);
	}

	//set opacity 
	void setAlpha(Uint8 alpha)
	{
		SDL_SetTextureAlphaMod(mTexture, alpha);
	}

	//set blend mode
	void setBlending(SDL_BlendMode blending)
	{
		SDL_SetTextureBlendMode(mTexture, blending);
	}

	//Draws texture
	void render(float x, float y, SDL_FRect* clip, float width, float height, double degrees, SDL_FPoint* center, SDL_FlipMode flipMode)
	{
		//Set texture position
		SDL_FRect dstRect{ x,y, static_cast<float>(mWidth), static_cast<float>(mHeight) };

		//default to clip dimensions if clip is given
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
		SDL_RenderTextureRotated(gRenderer, mTexture, clip, &dstRect, degrees, center, flipMode);
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
//LTexture gUpTexture, gDownTexture, gLeftTexture, gRightTexture;

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


	//if (gPngTexture.loadFromFile("assets/mona.bmp") == false)
	if (gPngTexture.loadFromFile("assets/up.png") == false)
	{
		SDL_Log("Unable to load png image!\n");
		success = false;
	}

	return success;
}

void close()
{
	//Clean up the surface
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

			//rotation degrees
			double degrees{ 0.0 };

			//flipmode
			SDL_FlipMode flipMode{ SDL_FLIP_NONE };

			//The currently rendered texture
			LTexture* currentTexture{ &gPngTexture };

			//Background color defaults to white
			SDL_Color bgColor{ 0xFF, 0xFF, 0xFF, 0xFF };
			//set color constants
			constexpr int kColorMagnitudeCount = 3;
			constexpr Uint8 kColorMagnitudes[kColorMagnitudeCount] = { 0x00, 0x7F, 0xFF };
			enum class eColorChannel
			{
				TextureRed = 0,
				TextureGreen = 1,
				TextureBlue = 2,
				TextureAlpha = 3,

				BackgroundRed = 4,
				BackgroundGreen = 5,
				BackgroundBlue = 6,

				Total = 7,
				Unknown = 8
			};

			//Initialize colors
			Uint8 colorChannelsIndices[static_cast<int>(eColorChannel::Total)];
			colorChannelsIndices[static_cast<int>(eColorChannel::TextureRed)] = 0;
			colorChannelsIndices[static_cast<int>(eColorChannel::TextureGreen)] = 0;
			colorChannelsIndices[static_cast<int>(eColorChannel::TextureBlue)] = 0;
			colorChannelsIndices[static_cast<int>(eColorChannel::TextureAlpha)] = 0;

			colorChannelsIndices[static_cast<int>(eColorChannel::BackgroundRed)] = 0;
			colorChannelsIndices[static_cast<int>(eColorChannel::BackgroundGreen)] = 0;
			colorChannelsIndices[static_cast<int>(eColorChannel::BackgroundBlue)] = 0;

			//Initialize blending
			gPngTexture.setBlending(SDL_BLENDMODE_BLEND);

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
						//check for channel key
						eColorChannel channelToUpdate = eColorChannel::Unknown;
						//Set texture 
						switch (e.key.key)
						{
							//Update texture color 
						case SDLK_A:
							channelToUpdate = eColorChannel::TextureRed;
							break;
						case SDLK_S:
							channelToUpdate = eColorChannel::TextureGreen;
							break;
						case SDLK_D:
							channelToUpdate = eColorChannel::TextureBlue;
							break;
						case SDLK_F:
							channelToUpdate = eColorChannel::TextureAlpha;
							break;

							//Update background color
						case SDLK_Q:
							channelToUpdate = eColorChannel::BackgroundRed;
							break;
						case SDLK_W:
							channelToUpdate = eColorChannel::BackgroundGreen;
							break;
						case SDLK_E:
							channelToUpdate = eColorChannel::BackgroundBlue;
							break;
							//Rotate on left/right press
						case SDLK_LEFT:
							degrees -= 36;
							break;
						case SDLK_RIGHT:
							degrees += 36;
							break;

							//Set flip mode based on 1/2/3 key press
						case SDLK_1:
							flipMode = SDL_FLIP_HORIZONTAL;
							break;
						case SDLK_2:
							flipMode = SDL_FLIP_NONE;
							break;
						case SDLK_3:
							flipMode = SDL_FLIP_VERTICAL;
							break;
						}
						//if channel key was pressed
						if (channelToUpdate != eColorChannel::Unknown)
						{
							//cycle through channel values
							colorChannelsIndices[static_cast<int>(channelToUpdate)]++;
							if (colorChannelsIndices[static_cast<int>(channelToUpdate)] >= kColorMagnitudeCount)
							{
								colorChannelsIndices[static_cast<int>(channelToUpdate)] = 0;
							}
							//Write color values to console
							SDL_Log("Texture - R:%d G:%d B:%d A:%d | Background - R:%d G:%d B:%d",
								kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::TextureRed)]],
								kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::TextureGreen)]],
								kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::TextureBlue)]],
								kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::TextureAlpha)]],
								kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::BackgroundRed)]],
								kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::BackgroundGreen)]],
								kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::BackgroundBlue)]]
							);

						}
					}
				}
				// reset background color
				bgColor.r = 0xFF;
				bgColor.g = 0xFF;
				bgColor.b = 0xFF;

				

				//Fill the background
				SDL_SetRenderDrawColor(gRenderer,
					kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::BackgroundRed)]],
					kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::BackgroundGreen)]],
					kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::BackgroundBlue)]],
					0xFF);
				SDL_RenderClear(gRenderer);

				//set texture color and render
				gPngTexture.setColor(
					kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::TextureRed)]],
					kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::TextureGreen)]],
					kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::TextureBlue)]]
				);

				gPngTexture.setAlpha(kColorMagnitudes[colorChannelsIndices[static_cast<int>(eColorChannel::TextureAlpha)]]);
				//gPngTexture.setBlending

				//Define center from corner of image
				SDL_FPoint center{ gPngTexture.getWidth() / 2.f, gPngTexture.getHeight() / 2.f };

				//Render image on screen
				currentTexture->render((kScreenWidth - currentTexture->getWidth()) / 2.f, (kScreenHeight - currentTexture->getHeight()) / 2.f, nullptr, LTexture::kOriginalSize, LTexture::kOriginalSize, degrees,&center,flipMode);

				//Update screen
				SDL_RenderPresent(gRenderer);

			}
			//Clean up (exit the loop)
			close();
			return exitCode;
		}
	}
}