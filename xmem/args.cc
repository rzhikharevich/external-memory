#include "args.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <sstream>


namespace xmem {


ArgParser::ArgParser(std::initializer_list<Entry> entries) {
    keyToValue.reserve(entries.size());

    size_t max_len = 0;

    for (const auto& entry : entries) {
        keyToValue[entry.longKey] = entry.value;

        size_t placeholderLen = entry.placeholder.empty() ?
            0 : (1 + entry.placeholder.size());

        max_len = std::max(max_len, entry.longKey.size() + placeholderLen);
    }

    std::ostringstream help_builder;
    help_builder << "Options:\n";

    for (const auto& entry : entries) {
        help_builder << "  " << entry.longKey;
        size_t len;

        if (entry.placeholder.empty()) {
            len = entry.longKey.size();
        } else {
            help_builder << ' ' << entry.placeholder;
            len = entry.longKey.size() + 1 + entry.placeholder.size();
        }

        for (size_t i = 0; i < max_len - len + 2; ++i) {
            help_builder << ' ';
        }

        help_builder << entry.help << '\n';
    }

    help = help_builder.str();
}

void ArgParser::parse(int argc, char* argv[]) const {
    bool ok = true;
    bool should_exit = false;
    size_t free_arg_idx = 0;

    for (int i = 1; i < argc; ++i) {
        String arg(argv[i]);

        if (arg == "-h" || arg == "--help") {
            should_exit = true;
        } else if (arg.starts_with("--")) {
            const auto value = keyToValue.get(arg);

            if (!value) {
                std::cerr << "Error: Unknown option '" << arg << "'.\n";
                ok = false;
            }

            ++i;

            if (i >= argc) {
                std::cerr << "Error: Expected an argument after '" << arg << "'.\n";
                ok = false;
                break;
            }

            if (!value) {
                continue;
            }

            **value = argv[i];
        } else if (free_arg_idx < fixed_free_args_.size()) {
            *fixed_free_args_[free_arg_idx] = arg;
            ++free_arg_idx;
        } else {
            std::cerr << "Error: Unexpected free argument << '" << arg << "'.\n";
        }
    }

    if (should_exit) {
        std::cerr << help;
    }

    if (!ok) {
        std::exit(1);
    }

    if (should_exit) {
        std::exit(0);
    }
}


}
