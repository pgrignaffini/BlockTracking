#include "stdafx.h"
#include "SoundManager.h"

void SoundManager::playSound(char note)
{
	// Load a sound buffer from a wav file
	sf::SoundBuffer buffer;
	std::string file;

	switch (note)
	{

		case 'a': file = "res/sounds/PianoNotes/a1.wav"; break;
		case 'b': file = "res/sounds/PianoNotes/b1.wav"; break;
		case 'c': file = "res/sounds/PianoNotes/c1.wav"; break;
		case 'd': file = "res/sounds/PianoNotes/d1.wav"; break;
		case 'e': file = "res/sounds/PianoNotes/e1.wav"; break;
		case 'f': file = "res/sounds/PianoNotes/f1.wav"; break;
		case 'g': file = "res/sounds/PianoNotes/g1.wav"; break;

		default:file = "res/sounds/error.wav";
		break;
	}


	if (!buffer.loadFromFile(file))
		return;

	// Display sound informations
	//std::cout << file << std::endl;
	//std::cout << " " << buffer.getDuration().asSeconds() << " seconds" << std::endl;
	//std::cout << " " << buffer.getSampleRate() << " samples / sec" << std::endl;
	//std::cout << " " << buffer.getChannelCount() << " channels" << std::endl;

	// Create a sound instance and play it
	sf::Sound sound(buffer);
	sound.play();

	// Loop while the sound is playing
	while (sound.getStatus() == sf::Sound::Playing)
	{
		// Leave some CPU time for other processes
		sf::sleep(sf::milliseconds(100));

		// Display the playing position
		std::cout << "\rPlaying... " << sound.getPlayingOffset().asSeconds() << " sec        ";
		std::cout << std::flush;
	}
	std::cout << std::endl << std::endl;
}


////////////////////////////////////////////////////////////
/// Play a music
///
////////////////////////////////////////////////////////////
void SoundManager::playMusic(const std::string& filename)
{
	// Load an ogg music file
	sf::Music music;
	if (!music.openFromFile("resources/" + filename))
		return;

	// Display music informations
	std::cout << filename << ":" << std::endl;
	std::cout << " " << music.getDuration().asSeconds() << " seconds" << std::endl;
	std::cout << " " << music.getSampleRate() << " samples / sec" << std::endl;
	std::cout << " " << music.getChannelCount() << " channels" << std::endl;

	// Play it
	music.play();

	// Loop while the music is playing
	while (music.getStatus() == sf::Music::Playing)
	{
		// Leave some CPU time for other processes
		sf::sleep(sf::milliseconds(100));

		// Display the playing position
		std::cout << "\rPlaying... " << music.getPlayingOffset().asSeconds() << " sec        ";
		std::cout << std::flush;
	}
	std::cout << std::endl << std::endl;
}


////////////////////////////////////////////////////////////
/// Entry point of application
///
/// \return Application exit code
///
////////////////////////////////////////////////////////////
/*
int main()
{
	// Play a sound
	playSound();

	// Play music from an ogg file
	playMusic("orchestral.ogg");

	// Play music from a flac file
	playMusic("ding.flac");

	// Wait until the user presses 'enter' key
	std::cout << "Press enter to exit..." << std::endl;
	std::cin.ignore(10000, '\n');

	return EXIT_SUCCESS;
}
*/

