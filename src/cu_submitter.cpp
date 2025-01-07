#include <iostream>
#include <QApplication>

#include "chgen/chgen.h"
#include "gui/cusubmittergui.h"
#include "transfer/transfer.h"
#include "submit/submit.h"
#include "utils/error.h"
#include "utils/print.h"

/**
 * @program cu_submitter
 * @brief The main entry point for the program.
 */
int main(int argc, char* argv[])
{
    if (argc < 2) {
        // App mode
        QApplication a(argc, argv);

        CUSubmitterGUI gui;

        log("Launching CU Submitter App");
        gui.show();
        return QApplication::exec();

    } else {
        //CLI mode
        const std::string option = argv[1];

        if (option == "--help" || option == "--usage") {
            std::string usage_message = "USAGE\n";
            usage_message += "-----\n";
            usage_message += "[-p <port>] : opens backend server on specific port; 3000 by default\n";
            usage_message += "--help | --usage : prints this message\n";
            usage_message += "--chgen <base_path> <modified_path> : generates a changelog text file\n";
            usage_message += "--transfer <unmodified_copy_path> <modified_copy_path> <destination_path> : transfers the modified files to the destination path\n";
            usage_message += "--submit <unmodified_copy_path> <modified_copy_path> [<archive_path>] : copy the modified files to a submission folder\n";

            print(usage_message);
        } else if (option == "--chgen") {
            if (argc < 4) {
                error("Not enough arguments");
                return 1;
            }

            const auto changelog = chgen::ChangelogGenerator::scan(argv[2], argv[3]);
            if (changelog == nullptr) {
                error("Could not generate changelog");
                return 1;
            }

            chgen::ChangelogGenerator::generate(changelog);
        } else if (option == "--transfer") {
            if (argc < 5) {
                error("Not enough arguments");
                return 1;
            }

            const std::string base = argv[2];
            const std::string from = argv[3];
            const std::string to = argv[4];

            const auto changelog = transfer::DevbuildTransferer::getTransferChangelog(base, from);

            if (changelog == nullptr) {
                error("Could not generate changelog");
                return 1;
            }

            std::cout << "Changelog: \n";
            std::cout << changelog->stringify() << "\n\n";

            std::cout << "Confirm transfer ? (O/N) ";
            char confirm;
            std::cin >> confirm;
            if (confirm == 'N' || confirm == 'n') {
                error("Transfer cancelled");
                return 8;
            }

            transfer::DevbuildTransferer::transfer(to);

            transfer::DevbuildTransferer::exportChangelog();
        } else if (option == "--submit") {
            if (argc < 4) {
                error("Not enough arguments");
                return 1;
            }

            const std::string base = argv[2];
            const std::string modified = argv[3];
            const std::string archive = argc >= 5 ? argv[4] : "";

            const auto changelog = submit::SubmissionBuilder::getSubmissionChangelog(base, modified);

            if (changelog == nullptr) {
                error("Could not generate changelog");
                return 1;
            }

            std::cout << "Changelog: \n";
            std::cout << changelog->stringify() << "\n\n";

            std::cout << "Confirm ? (O/N) ";
            char confirm;
            std::cin >> confirm;
            if (confirm == 'N' || confirm == 'n') {
                error("Submission cancelled");
                return 8;
            }

            try {
                if (!archive.empty()) {
                    submit::SubmissionBuilder::submit(archive);
                } else {
                    submit::SubmissionBuilder::submit();
                }

                //submit::SubmissionBuilder::compress();
            } catch (const std::exception &e) {
                error(std::string(e.what()));
            }
        }

        return 0;
    }
}
