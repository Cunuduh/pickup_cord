#include <array>
#include <boost/json.hpp>
#include <boost/json/src.hpp>
#include <dpp/dpp.h>
#include <iostream>
#include <string>

namespace commands {
    class pickup_line_command {
        dpp::cluster &bot;
        const dpp::slashcommand_t &event;
        const dpp::user &user;
        boost::json::array _create_request_object(const std::string &prompt);
        std::string _create_chat_completion(const std::string &model, boost::json::array messages);
        public:
            pickup_line_command(dpp::cluster &bot, const dpp::slashcommand_t &event, const dpp::user &user) : bot(bot), event(event), user(user) {};
            void send_pickup_line(std::string &prompt) {
                auto chatgpt = _create_chat_completion("gpt-3.5-turbo", _create_request_object(prompt));
                boost::json::object chatgpt_response = boost::json::parse(chatgpt).as_object();
                std::string content = chatgpt_response["choices"].as_array()[0].as_object()["message"].as_object()["content"].as_string().c_str();
                event.reply(content);
            }
        private:
            boost::json::array _create_request_object(const std::string &prompt) {
                return boost::json::array({
                    boost::json::object {
                        {"role", "system"},
                        {"content", "You are a bot that generates a single pickup line based on the prompt given by the user. Reject the prompt if it is not a pickup line."}
                    },
                    boost::json::object {
                        {"role", "user"},
                        {"content", prompt}
                    }
                });
            }
            std::string _create_chat_completion(const std::string &model, boost::json::array messages) {
                boost::json::object chat_completion = {
                    {"model", model},
                    {"messages", messages}
                };
                std::string chat_completion_response;
                bot.request("https://api.openai.com/v1/chat/completions", dpp::http_method::m_post, [&chat_completion_response](const dpp::http_request_completion_t &cc) {
                    if (cc.error) {
                        std::cerr << "Error: " << cc.status << std::endl;
                    } else {
                        chat_completion_response = cc.body;
                    }
                }, boost::json::serialize(chat_completion), "application/json", {{"Authorization", "Bearer " + std::string(getenv("API_KEY"))}});
                return chat_completion_response;
            }
    };
}