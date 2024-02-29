import argparse
import json
import os


def directory(string:str):
    if string == "":
        return ""

    if os.path.isdir(string):
        return string
    else:
        raise argparse.ArgumentTypeError(f"'{string}' is not a valid directory.")


def sanitizepath(path:str, file:str, access_type : str = 'r'):
    result = os.path.join(path, file)

    if (access_type == 'r'):
        to_verify = result
    else:
        to_verify = os.path.dirname(result)

        if to_verify == "":
            return result

    if os.path.exists(to_verify):
        return result
    else:
        raise argparse.ArgumentTypeError(f"'{result}' is not a valid file path.")


def uint(value:str):
    ivalue = int(value)
    if ivalue <= 0:
        raise argparse.ArgumentTypeError("%s is an invalid positive int value" % value)
    return ivalue



if __name__ == "__main__":
    parser = argparse.ArgumentParser(add_help=False)
    parser.add_argument("--json_config_file", type=lambda x: sanitizepath("", x), help="JSON config file")
    parser.add_argument("--root_folder", type=directory, help="Folder containing all used files", default="")
    args = parser.parse_known_args()[0]
    parser = argparse.ArgumentParser(description="This is a Poc of Argparse with Json config file", parents=[parser])
    parser.add_argument("--train_file", type=lambda x: sanitizepath(args.root_folder, x), help="File with train data")
    parser.add_argument("--test_file", type=lambda x: sanitizepath(args.root_folder, x), help="File with test data")
    parser.add_argument("--pred_file", type=lambda x: sanitizepath(args.root_folder, x), help="File with predictions data")
    parser.add_argument("--console_file", type=lambda x: sanitizepath(args.root_folder, x, 'w'), help="File with terminal logs redirection")
    parser.add_argument("--mus", type=float, nargs='+', help="List of floats", default=None)
    parser.add_argument("--sigmas", type=float, nargs='+', help="List of floats", default=None)
    parser.add_argument("--nb_threads", type=int, help="nb CPU used", default=1, choices=range(1, 9), metavar="[1-8]")
    parser.add_argument("--search_strategy", type=bool, help="If using dichotomic search", default=True)
    # parser.add_argument("-h","--help", action=parser.print_help)

    print(args)

    # TODO fix this to work with json config files
    if args.json_config_file is not None:
        print(args.json_config_file)
        with open(args.json_config_file, "r") as f:
            res = json.load(f)
            parser.parse_args(**res)

    args = parser.parse_args()
    print(args)
