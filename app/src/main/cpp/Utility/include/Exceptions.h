//
// Created by Petr Flajsingr on 12/05/2018.
//

#ifndef SPEECHRECOGNITION_EXCEPTIONS_H
#define SPEECHRECOGNITION_EXCEPTIONS_H

#include <exception>
#include <string>

namespace SpeechRecognition::Exceptions {
    class ASRFilesMissingException : public std::exception {
    protected:
        std::string message;
    public:
        explicit ASRFilesMissingException(const char* errorMessage):message(errorMessage){}

        explicit ASRFilesMissingException(const std::string& errorMessage):message(errorMessage){}

        virtual ~ASRFilesMissingException() throw(){}

        virtual const char* what() const throw(){
            return message.c_str();
        }
    };

}


#endif //SPEECHRECOGNITION_EXCEPTIONS_H
