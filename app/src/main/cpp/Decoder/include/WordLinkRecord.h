//
// Created by Petr Flajsingr on 20/04/2018.
//

#ifndef VOICERECOGNITION_WORDLISTRECORD_H
#define VOICERECOGNITION_WORDLISTRECORD_H

#include "LMWord.h"

namespace SpeechRecognition::Decoder {

    /**
     * @brief Class for saving history of passed words.
     *
     * Counts its reference count and deletes itself when it's no longer in use.
     *
     * @author Petr Flajšingr, xflajs00@stud.fit.vutbr.cz
     */
    class WordLinkRecord {
    private:
        unsigned int referenceCount = 1;
    public:
        /**
         * Links the record to older ones
         * @param previous previous record
         * @param word word to represent
         */
        WordLinkRecord(WordLinkRecord *previous, LMWord* word);

        virtual ~WordLinkRecord();

        LMWord* word;

        WordLinkRecord* previous;

        /**
         * Needs to be called insted of "=" to increase reference count
         * @return this object
         */
        WordLinkRecord* assign();

        /**
         * Needs to be called after unassignin the pointer to decrease reference count.
         * Deletes the object when reference count reaches 0
         */
        void unassign();

        /**
         * Creates new records and links it to this one.
         * @param word new word
         * @return new record
         */
        WordLinkRecord* addRecord(LMWord* word);

        float getBigramProbability();
    };
}

#endif //VOICERECOGNITION_WORDLISTRECORD_H
