#pragma once
#include <SFML/Audio.hpp>
#include <iostream>
#include <string>
#include <thread>

class SoundManager
{
public:
	void playSound(const std::string& filename);
	void playMusic(const std::string& filename);

};

