//
// Created by Petr Flajsingr on 30/03/2018.
//

#include <string>
#include <AcousticModel.h>
#include <fstream>
#include <Utils.h>
#include <cstdlib>
#include <android/asset_manager.h>

/**
 * Reads a lexicon from file.
 * @param path path to lexicon file
 */
AcousticModel::AcousticModel(std::string path) {
    std::ifstream file;

    file.open(path.c_str(), std::ios::in|std::ios::binary);
    if(file.is_open()){
        uint32_t wordCount;
        file.read((char*)(&wordCount), 4);


        for(int i = 0; i < wordCount; i++){
            uint8_t wordLength;
            file.read((char*)(&wordLength), 1);
            char word[wordLength + 1];
            for(int i = 0; i < wordLength; i++)
                file.read(&word[i], 1);
            word[wordLength] = '\0';

            std::string strWord = word;
            Word amWord(word);

            uint8_t phonemeCount;
            file.read((char*)(&phonemeCount), 1);
            uint8_t phoneme;

            for(int j = 0; j < phonemeCount;j++) {
                file.read((char*)(&phoneme), 1);
                amWord.phonemes.push_back((PHONEME_ENUM)phoneme);
            }
            this->words.push_back(amWord);
        }
    }
    file.close();
}
