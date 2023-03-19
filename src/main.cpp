#include <cstdlib>
#include <dpp/dpp.h>
#include <string>
#include "commands/pickup_line.hpp"

using namespace dpp;
using namespace std;

int main() {
    const char *token = getenv("DISCORD_TOKEN");
    if (!token) {
        cerr << "DISCORD_TOKEN environment variable not set" << endl;
        return 1;
    }
    cluster bot(token);
    bot.on_log(utility::cout_logger());
    bot.on_slashcommand([&bot](const dpp::slashcommand_t &event) {
        if (event.command.get_command_name() == "pickup_line") {
            string prompt = event.command.get_command_interaction().options[0].get_value<string>(0);
            commands::pickup_line_command pickup_line(bot, event, event.command.get_issuing_user());
            pickup_line.send_pickup_line(prompt);
        }
    });
    bot.on_ready([&bot](const dpp::ready_t &event) {
        if (dpp::run_once<struct register_bot_commands>()) {
            slashcommand pickup_line("pickup_line", "Generate a pickup line", bot.me.id);
            pickup_line.add_option(command_option(command_option_type::co_string, "prompt", "A prompt the pickup line is based upon. Ex: \"She's a 10, but . . .\"", true));

            bot.global_command_create(pickup_line);
        }
    });
    bot.start(st_wait);
    return 0;
}