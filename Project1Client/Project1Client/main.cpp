/// <summary>
/// Three player tag game using winsock
/// 
/// 
/// @author Cliona Hayden
/// @date May 21017
/// </summary>

#include <SFML/Graphics.hpp>


#ifdef _DEBUG 
#pragma comment(lib,"sfml-graphics-d.lib") 
#pragma comment(lib,"sfml-audio-d.lib") 
#pragma comment(lib,"sfml-system-d.lib") 
#pragma comment(lib,"sfml-window-d.lib") 
#pragma comment(lib,"sfml-network-d.lib") 
#pragma comment(lib,"ws2_32.lib") 
#else 
#pragma comment(lib,"sfml-graphics.lib") 
#pragma comment(lib,"sfml-audio.lib") 
#pragma comment(lib,"sfml-system.lib") 
#pragma comment(lib,"sfml-window.lib") 
#pragma comment(lib,"sfml-network.lib") 
#pragma comment(lib,"ws2_32.lib") 
#endif 

#include "Game.h"

/// <summary>
/// main enrtry point
/// </summary>
/// <returns>zero</returns>

int main()
{
	Game game;
	game.run();

	return 1;
}