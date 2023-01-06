#ifndef translation_h
#define translation_h

#include <string>
#include <map>
#include <vector>

#include <wx/string.h>

/**
 * @class Translator a simple translator that can be easily upgraded to read from a file.
 * Right now the translations are directly in RAM but since the program is small that's OK.
 * Downside is we need to recompile to add things.
 * 
*/
class Translator
{
public:
    typedef enum
    {
        LANG_INVALID = -1,
        LANG_EN = 0,
        LANG_FR,
        LANG_SIZE
    } LANG_t;
    Translator(LANG_t pLang = LANG_t::LANG_EN);
    const wxString &getWord(const char *pWord) const;
    const wxString &getWord(const std::string &pWord) const;
    LANG_t getLang() const;
    void setLang_rebootRequired(LANG_t pLang);

    /**
     * Debug functions used to get the content of _getWords()
    */
    const wxString &__getWord_fill_map(const char *pWord);
    const wxString &__getWord_fill_map(const std::string &pWord);

private:
    LANG_t lang;

    class TranslationStrings
    {
    public:
        TranslationStrings(std::initializer_list<const char *> pWords);
        TranslationStrings(unsigned int n, const char *pWord);
        const std::vector<wxString> &getStrings() const;

    private:
        std::vector<wxString> strings;
    };
    std::map<std::string, TranslationStrings> _getWords() const;
    std::map<std::string, TranslationStrings> words = _getWords();
};

extern Translator *_translator;

#define __TUTF8(word) _translator->getWord(word)
#define INIT_TRANSLATOR(LANG) _translator = new Translator(LANG)
#define CURRENT_LANG() _translator->getLang()
#define SET_LANG_BEFORE_REBOOT(LANG) _translator->setLang_rebootRequired(LANG)

#endif