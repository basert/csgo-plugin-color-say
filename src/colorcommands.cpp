#include "colorcommands.h"
#include "globals.h"
#include "constants.h"
#include "chatcolors.h"
#include "chat.h"
#include <unordered_map>
#include <sstream>
#include <algorithm>

using namespace std;

namespace colorsay {

    namespace colorcommands {

        static unordered_map<string, ColorCommand *> _commands;

        bool exists(const string &name) {
            string lowername = name;
            std::transform(lowername.begin(), lowername.end(), lowername.begin(), ::tolower);
            return _commands.count(lowername);
        }

        PLUGIN_RESULT invoke(edict_t *pEdict, const string &name, const string &args, const vector<string> &argv) {
            string lowername = name;
            std::transform(lowername.begin(), lowername.end(), lowername.begin(), ::tolower);
            return _commands.at(lowername)->invoke(pEdict, args, argv);
        }

    }

    class HelpCommand : public ColorCommand {
    public:
        virtual const string get_name() const {
            return "help";
        }

        virtual const string get_description() const {
            return "Prints a list of available commands, or more specific help on a command";
        }

        virtual const string get_usage() const {
            return "help <command>";
        }

        virtual const string get_help() const {
            return "Prints a list of available commands, or more specific help on a command";
        }

        virtual PLUGIN_RESULT invoke(edict_t *pEdict, const string &args, const vector<string> &argv) const {
            ostringstream ss;
            if (argv.size() == 2) {
                const string &name = argv[1];
                if(!colorcommands::exists(name)) {
                    ss << "Unknown command \"" << name << "\"\n";
                    Globals::pEngine->ClientPrintf(pEdict, ss.str().c_str());
                } else {
                    const ColorCommand *cmd = colorcommands::_commands.at(name);
                    ss << "Usage: " << cmd->get_usage() << "\n\n" << cmd->get_help() << "\n";
                    Globals::pEngine->ClientPrintf(pEdict, ss.str().c_str());
                }
            } else if (argv.size() == 1) {
                Globals::pEngine->ClientPrintf(pEdict, "GitHub: github.com/burnedram/csgo-plugin-color-say\n\nAvailable commands:\n");
                for (auto &pair : colorcommands::_commands) {
                    auto &cc = pair.second;
                    Globals::pEngine->ClientPrintf(pEdict, "\t");
                    Globals::pEngine->ClientPrintf(pEdict, cc->get_name().c_str());
                    Globals::pEngine->ClientPrintf(pEdict, ": ");
                    Globals::pEngine->ClientPrintf(pEdict, cc->get_description().c_str());
                    Globals::pEngine->ClientPrintf(pEdict, "\n");
                }
            } else {
                ss << "Usage: " << get_usage() << "\n";
                Globals::pEngine->ClientPrintf(pEdict, ss.str().c_str());
            }
            return PLUGIN_STOP;
        }
    };

    class VersionCommand : public ColorCommand {
    public:
        virtual const string get_name() const {
            return "version";
        }

        virtual const string get_description() const {
            return "Prints the version of this plugin";
        }

        virtual const string get_usage() const {
            return "version";
        }

        virtual const string get_help() const {
            return "Prints the version of this plugin";
        }

        virtual PLUGIN_RESULT invoke(edict_t *pEdict, const string &args, const vector<string> &argv) const {
            Globals::pEngine->ClientPrintf(pEdict, "Plugin version " PLUGIN_VERSION "\n");
            ostringstream ss;
            ss << "[" << chatcolor::random() << PLUGIN_NAME << chatcolor::ID::WHITE << "] Plugin version " PLUGIN_VERSION;
            chat::say(pEdict, ss.str());
            return PLUGIN_STOP;
        }
    };

    class AvailableColorsCommand : public ColorCommand {
    public:
        virtual const string get_name() const {
            return "list";
        }

        virtual const string get_description() const {
            return "Lists all available colors";
        }

        virtual const string get_usage() const {
            return "list";
        }

        virtual const string get_help() const {
            return "Lists all available colors";
        }

        virtual PLUGIN_RESULT invoke(edict_t *pEdict, const string &args, const vector<string> &argv) const {
            ostringstream ss;
            ostringstream ss2;
            string text;
            chatcolor::RGB rgb;
            chatcolor::ID color;

            ss << "[" << chatcolor::random() << PLUGIN_NAME << chatcolor::ID::WHITE << "] Available colors";
            text = ss.str();
            chat::say(pEdict, text);
            Globals::pEngine->ClientPrintf(pEdict, text.c_str());
            Globals::pEngine->ClientPrintf(pEdict, "\n");

            ss.str("");
            for (color = chatcolor::min; color < chatcolor::max; color++) {
                ss2 << "(" << (int)color << ") " << color << chatcolor::name(color) << "\x01, ";
                ss << ss2.str();
                rgb = chatcolor::rgb(color);
                ss2 << "r" << (int)rgb.r;
                ss2 << " g" << (int)rgb.g;
                ss2 << " b" << (int)rgb.b;
                Globals::pEngine->ClientPrintf(pEdict, ss2.str().c_str());
                Globals::pEngine->ClientPrintf(pEdict, "\n");
                ss2.str("");

                if ((color - chatcolor::min) % 2 == 1) {
                    chat::say(pEdict, ss.str());
                    ss.str("");
                }
            }
            ss2 << "(" << (int)color << ") " << color << chatcolor::name(color);
            ss << ss2.str();
            ss2 << chatcolor::ID::WHITE << ", ";
            rgb = chatcolor::rgb(color);
            ss2 << "r" << (int)rgb.r;
            ss2 << " g" << (int)rgb.g;
            ss2 << " b" << (int)rgb.b;
            Globals::pEngine->ClientPrintf(pEdict, ss2.str().c_str());
            Globals::pEngine->ClientPrintf(pEdict, "\n");

            chat::say(pEdict, ss.str());

            return PLUGIN_STOP;
        }
    };

    namespace colorcommands {

        void register_commands() {
            for (auto cc :  initializer_list<ColorCommand *>({
                        new HelpCommand(), new VersionCommand(),
                        new AvailableColorsCommand()}))
                _commands[cc->get_name()] = cc;
        }

    }

}
