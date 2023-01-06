#include "translation.h"

static wxString __undefined = "?";

Translator::Translator(LANG_t pLang)
    : lang(pLang)
{
}
const wxString &Translator::getWord(const char *pWord) const
{
    const auto found = words.find(pWord);
    if (found == words.end())
    {
        printf("%s not found\n", pWord);
        __undefined = wxString::FromUTF8(pWord);
        return __undefined;
    }
    return found->second.getStrings().at(lang);
}
const wxString &Translator::getWord(const std::string &pWord) const
{
    return getWord(pWord.c_str());
}
Translator::LANG_t Translator::getLang() const
{
    return lang;
}
void Translator::setLang_rebootRequired(Translator::LANG_t pLang)
{
    lang = pLang;
}

const wxString &Translator::__getWord_fill_map(const char *pWord)
{
    const auto found = words.find(pWord);
    if (found == words.end())
    {
        const auto i = words.insert({pWord, Translator::TranslationStrings(LANG_t::LANG_SIZE, pWord)});
        return i.first->second.getStrings().at(lang);
    }
    return found->second.getStrings().at(lang);
}
const wxString &Translator::__getWord_fill_map(const std::string &pWord)
{
    return __getWord_fill_map(pWord.c_str());
}

std::map<std::string, Translator::TranslationStrings> Translator::_getWords() const
{
    return {
        {"Name:", {"Name:", "Nom:"}},
        {"Catalog N.:", {"Catalog N.:", "# Catalogue:"}},
        {"minID (mindat):", {"minID (mindat):", "minID (mindat):"}},
        {"Locality:", {"Locality:", "Localité:"}},
        {"Locality ID (mindat):",
         {"Locality ID (mindat):", "ID de Localité (mindat):"}},
        {"mm", {"mm", "mm"}},
        {"cm", {"cm", "cm"}},
        {"m", {"m", "m"}},
        {"inches", {"inches", "pouces"}},
        {"feet", {"feet", "pieds"}},
        {"Size:", {"Size:", "Grosseur:"}},
        {"Micromount (microscope)", {"Micromount (microscope)", "Micromount (microscope)"}},
        {"Thumbnail (<1 inch)", {"Thumbnail (<1 inch)", "Vignette (<1 pouce)"}},
        {"Toenail (<1.25 inch)", {"Toenail (<1.25 inch)", "Onglet (<1.25 pouce)"}},
        {"Miniature (3-6 cm)", {"Miniature (3-6 cm)", "Miniature (3-6 cm)"}},
        {"Small cabinet (5-7 cm)", {"Small cabinet (5-7 cm)", "Petit Cabinet (5-7 cm)"}},
        {"Cabinet (6-10 cm)", {"Cabinet (6-10 cm)", "Cabinet (6-10 cm)"}},
        {"Large cabinet (10-15 cm)", {"Large cabinet (10-15 cm)", "Large cabinet (10-15 cm)"}},
        {"Extra Large (>15 cm)", {"Extra Large (>15 cm)", "Très Large (>15 cm)"}},
        {"g", {"g", "g"}},
        {"mg", {"mg", "mg"}},
        {"kg", {"kg", "kg"}},
        {"oz", {"oz", "oz"}},
        {"lb", {"lb", "lb"}},
        {"carats", {"carats", "carats"}},
        {"Weight:", {"Weight:", "Poids:"}},
        {"January", {"January", "Janvier"}},
        {"February", {"February", "Février"}},
        {"March", {"March", "Mars"}},
        {"April", {"April", "Avril"}},
        {"May", {"May", "Mai"}},
        {"June", {"June", "Juin"}},
        {"July", {"July", "Juillet"}},
        {"August", {"August", "Aout"}},
        {"September", {"September", "Septembre"}},
        {"October", {"October", "Octobre"}},
        {"November", {"November", "Novembre"}},
        {"December", {"December", "Decembre"}},
        {"Acquisition:", {"Acquisition:", "Acquisition:"}},
        {"From:", {"From:", "De:"}},
        {"Deaccessioned:", {"Deaccessioned:", "Abandonné:"}},
        {"To:", {"To:", "À:"}},
        {"Collection:", {"Collection:", "Collection:"}},
        {"Self collected:", {"Self collected:", "Auto-collecté:"}},
        {"self-collected", {"self-collected", "auto-collecté"}},
        {"See on MINDAT", {"See on MINDAT", "Voir sur MINDAT"}},
        {"Estimated value:", {"Estimated value:", "Valeur estimée:"}},
        {"Price paid:", {"Price paid:", "Prix payé:"}},
        {"Species 1", {"Species 1", "Espèce 1"}},
        {"Species 2", {"Species 2", "Espèce 2"}},
        {"Species 3", {"Species 3", "Espèce 3"}},
        {"Species 4", {"Species 4", "Espèce 4"}},
        {"Species:", {"Species:", "Espèces:"}},
        {"Variety:", {"Variety:", "Variété:"}},
        {"Class:", {"Class:", "Classe:"}},
        {"Chemical Formula:", {"Chemical Formula:", "Formule Chimique:"}},
        {"Color:", {"Color:", "Couleur:"}},
        {"Opaque", {"Opaque", "Opaque"}},
        {"Translucent", {"Translucent", "Translucide"}},
        {"Transparent", {"Transparent", "Transparent"}},
        {"Transparency:", {"Transparency:", "Transparence:"}},
        {"Habit:", {"Habit:", "Habitus:"}},
        {"Fluorescence (SW):", {"Fluorescence (SW):", "Fluorescence (SW):"}},
        {"Fluorescence (MW):", {"Fluorescence (MW):", "Fluorescence (MW):"}},
        {"Fluorescence (LW):", {"Fluorescence (LW):", "Fluorescence (LW):"}},
        {"Fluorescence (405nm):", {"Fluorescence (405nm):", "Fluorescence (405nm):"}},
        {"Phosphorescence (SW):", {"Phosphorescence (SW):", "Phosphorescence (SW):"}},
        {"Phosphorescence (MW):", {"Phosphorescence (MW):", "Phosphorescence (MW):"}},
        {"Phosphorescence (LW):", {"Phosphorescence (LW):", "Phosphorescence (LW):"}},
        {"Phosphorescence (405nm):", {"Phosphorescence (405nm):", "Phosphorescence (405nm):"}},
        {"Tenebrescence:", {"Tenebrescence:", "Ténébrescence:"}},
        {"Triboluminescence:", {"Triboluminescence:", "Triboluminescence:"}},
        {"Radioactivity:", {"Radioactivity:", "Radioactivité:"}},
        {"Description:", {"Description:", "Description:"}},
        {"Notes:", {"Notes:", "Notes:"}},
        {"Previous owners:", {"Previous owners:", "Précédents propriétaires:"}},
        {"Save", {"Save", "Sauvegarder"}},
        {"Cancel", {"Cancel", "Annuler"}},
        {"Radioactivity    : ", {"Radioactivity    : ", "Radioactivité    : "}},
        {"Description: ", {"Description: ", "Description: "}},
        {"Notes: ", {"Notes: ", "Notes: "}},
        {"Previous owners: ", {"Previous owners: ", "Précédents propriétaires: "}},
        {"Do you want to generate a report for the full database or only the selected mineral?", {"Do you want to generate a report for the full database or only the selected mineral?", "Voulez-vous générer un rapport pour l'ensemble de la base de données ou seulement pour le minéral sélectionné?"}},
        {"Full database", {"Full database", "Toute la base de données"}},
        {"Selected mineral", {"Selected mineral", "Minéral sélectioné"}},
        {"Do you want to include the images and data?", {"Do you want to include the images and data?", "Voulez-vous inclure les images et données?"}},
        {"Yes, please include the images and data", {"Yes, please include the images and data", "Oui, inclure les images et données."}},
        {"No, thanks", {"No, thanks", "Non, merci"}},
        {"The generated HTML report can be opened in any internet browser, or also in Microsoft Word.", {"The generated HTML report can be opened in any internet browser, or also in Microsoft Word.", "Le rapport HTML généré peut être ouvert avec un navigateur internet ou encore Microsoft Word."}},
        {"Import mineral database from a CSV file", {"Import mineral database from a CSV file", "Import mineral database from a CSV file"}},
        {"Import CSV", {"Import CSV", "Importer CSV"}},
        {"Export mineral database as CSV file", {"Export mineral database as CSV file", "Exporter la base de données en tant que fichier CSV"}},
        {"Export CSV", {"Export CSV", "Exporter CSV"}},
        {"Add a new mineral to the database", {"Add a new mineral to the database", "Ajoute un nouveau minéral à la base de données"}},
        {"Add", {"Add", "Ajouter"}},
        {"Modify the selected mineral", {"Modify the selected mineral", "Modifie le minéral sélectionné"}},
        {"Modify", {"Modify", "Modifier"}},
        {"Duplicate the selected mineral", {"Duplicate the selected mineral", "Duplique le minéral sélectionné"}},
        {"Duplicate", {"Duplicate", "Dupliquer"}},
        {"Delete the selected mineral", {"Delete the selected mineral", "Supprime le minéral sélectionné"}},
        {"Delete", {"Delete", "Supprimer"}},
        {"Hide mineral value", {"Hide mineral value", "Cacher les informations"}},
        {"Generate printable report", {"Generate printable report", "Générer un rapport imprimable"}},
        {"Open the MineralApp manual on the browser", {"Open the MineralApp manual on the browser", "Ouvrir le manuel de MineralApp"}},
        {"Read the manual online", {"Read the manual online", "Lire le manuel en ligne"}},
        {"File", {"File", "Fichier"}},
        {"Mineral", {"Mineral", "Minéral"}},
        {"Help", {"Help", "Aide"}},
        {"Welcome to MineralApp!", {"Welcome to MineralApp!", "Bienvenue sur MineralApp!"}},
        {"Filter:", {"Filter:", "Filtre:"}},
        {"Order by:", {"Order by:", "Ordonné par:"}},
        {"Unique ID", {"Unique ID", "ID Unique"}},
        {"Name", {"Name", "Nom"}},
        {"Country:", {"Country:", "Pays:"}},
        {"Species:", {"Species:", "Espèces:"}},
        {"Question", {"Question", "Question"}},
        {"You have already an open database. Do you want to discard it and open a new one?",
         {"You have already an open database. Do you want to discard it and open a new one?",
          "Une base de donnée est déjà ouverte. Souhaitez-vous la jeter et en ouvrir une nouvelle?"}},
        {"Database files (*.sqlite3)|*.sqlite3", {"Database files (*.sqlite3)|*.sqlite3", "Fichiers (*.sqlite3)|*.sqlite3"}},
        {"Save database", {"Save database", "Sauvegarder la base de données"}},
        {"You are going to close the current database. If you did not save it any edits will be lost. This operation cannot be undone.",
         {"You are going to close the current database. If you did not save it any edits will be lost. This operation cannot be undone.",
          "La base de donnée présente sera fermée. Tous les changements seront perdus. Cette opération est irréversible."}},
        {"Are you sure you want to close the current database?",
         {"Are you sure you want to close the current database?",
          "Voulez-vous vraiment fermer la base de données actuelle?"}},
        {"Add new mineral", {"Add new mineral", "Ajouter un nouveau minéral"}},
        {"Please, select a mineral from the left panel.",
         {"Please, select a mineral from the left panel.",
          "Veuillez sélectionner un minéral du panneau de gauche."}},
        {"Mineral duplicated!", {"Mineral duplicated!", "Minéral dupliqué!"}},
        {". This operation cannot be undone.", {". This operation cannot be undone.", ". Cette opération est irréversible."}},
        {"You are going to delete mineral ID ", {"You are going to delete mineral ID ", "Vous allez supprimer le minéral avec ID "}},
        {"Are you sure you want to delete this mineral?", {"Are you sure you want to delete this mineral?", "Voulez-vous supprimer ce minéral?"}},
        {"Catalog Number   : ", {"Catalog Number   : ", "# Catalogue      : "}},
        {"Locality         : ", {"Locality         : ", "Localité         : "}},
        {"Size             : ", {"Size             : ", "Grosseur         : "}},
        {"Weight           : ", {"Weight           : ", "Poids            : "}},
        {"Acquisition      : ", {"Acquisition      : ", "Acquisition      : "}},
        {"Collection       : ", {"Collection       : ", "Collection       : "}},
        {"Estimated Value  : ", {"Estimated Value  : ", "Valeur Estimée   : "}},
        {"Species          ", {"Species          ", "Espèces          "}},
        {"Variety          ", {"Variety          ", "Variété          "}},
        {"Class            ", {"Class            ", "Classe           "}},
        {"Chem. Formula    ", {"Chem. Formula    ", "Formule Ch.      "}},
        {"Color            ", {"Color            ", "Couleur          "}},
        {"Transparency     ", {"Transparency     ", "Transparence     "}},
        {"Habit            ", {"Habit            ", "Habitus          "}},
        {"Fluorescence SW  ", {"Fluorescence SW  ", "Fluorescence SW  "}},
        {"Fluorescence MW  ", {"Fluorescence MW  ", "Fluorescence MW  "}},
        {"Fluorescence LW  ", {"Fluorescence LW  ", "Fluorescence LW  "}},
        {"Fluor. 405nm     ", {"Fluor. 405nm     ", "Fluor. 405nm     "}},
        {"Phosphor. SW     ", {"Phosphor. SW     ", "Phosphor. SW     "}},
        {"Phosphor. MW     ", {"Phosphor. MW     ", "Phosphor. MW     "}},
        {"Phosphor. LW     ", {"Phosphor. LW     ", "Phosphor. LW     "}},
        {"Phosphor. 405nm  ", {"Phosphor. 405nm  ", "Phosphor. 405nm  "}},
        {"Tenebrescence    ", {"Tenebrescence    ", "Ténébrescence    "}},
        {"Triboluminescence", {"Triboluminescence", "Triboluminescence"}},
        {"Figures & Data :", {"Figures & Data :", "Figures & Données :"}},
        {"You have already an open database. By importing from a CSV file any duplicate mineral id will overwrite existing ones. Do you want to continue?",
         {"You have already an open database. By importing from a CSV file any duplicate mineral id will overwrite existing ones. Do you want to continue?",
          "Une base de données est déjà ouverte. En importer une nouvelle depuis un fichier CSV écrasera tous les minéraux avec un ID dupliqué. Voulez-vous continuer?"}},
        {"CSV files (*.csv)|*.csv", {"CSV files (*.csv)|*.csv", "Fichiers CSV (*.csv)|*.csv"}},
        {"Export CSV file", {"Export CSV file", "Exporter fichier CSV"}},
        {"Generate printable report", {"Generate printable report", "Générer un rapport imprimable"}},
        {"HTML files (*.html)|*.html", {"HTML files (*.html)|*.html", "Fichiers HTML (*.html)|*.html"}},
        {"Generate Report", {"Generate Report", "Générer un rapport"}},
        {"Writing failed!", {"Writing failed!", "L'écriture a échouée!"}},
        {"Sorry, no mineral was selected. Select one from the left panel.",
         {"Sorry, no mineral was selected. Select one from the left panel.", "Désolé, aucun minéral n'a été sélectionné dans le panneau de gauche."}},
        {"Language", {"Language", "Langue"}},
        {"English", {"English", "Anglais"}},
        {"Select English", {"Select English", "Sélectionner Anglais"}},
        {"French", {"French", "Français"}},
        {"Select French", {"Select French", "Sélectionner Français"}},
        {"MineralApp must close to change the language. All edits will be lost if you did not save them. Close MineralApp?", {"MineralApp must close to change the language. All edits will be lost if you did not save them. Close MineralApp?", "MineralApp doit être fermé pour appliquer les changements. Les modifications non sauvegardées seront perdues. Voulez-vous fermer MineralApp?"}},
        {"Are you sure you want to close MineralApp?", {"Are you sure you want to close MineralApp?", "Voulez-vous fermer MineralApp?"}},
        {"Open", {"Open", "Ouvrir"}},
        {"Save", {"Save", "Sauvegarder"}},
        {"Close", {"Close", "Fermer"}},
        {"Exit", {"Exit", "Quitter"}},
        {"Yes", {"Yes", "Oui"}},
        {"No", {"No", "Non"}},
        {"Open database", {"Open database", "Ouvrir une base de données"}},
        {"<hidden>", {"<hidden>", "<masqué>"}},
        {"price paid", {"price paid", "prix payé"}}};
}

Translator::TranslationStrings::TranslationStrings(std::initializer_list<const char *> pWords)
{
    for (auto i = pWords.begin(); i != pWords.end(); i++)
    {
        this->strings.push_back(wxString::FromUTF8(*i));
    }
}
Translator::TranslationStrings::TranslationStrings(unsigned int n, const char *pWord)
{
    for (unsigned int i = 0; i < n; i++)
    {
        this->strings.push_back(wxString::FromUTF8(pWord));
    }
}
const std::vector<wxString> &Translator::TranslationStrings::getStrings() const
{
    return this->strings;
}

Translator *_translator = NULL;