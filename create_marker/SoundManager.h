#pragma once
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>

class SoundManager
{
public:
	void playSound(char note);
	void playMusic(const std::string& filename);

};

