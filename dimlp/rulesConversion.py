def rulesConversion(rules_file, output_file):
    rules = []
    try:
        with open(rules_file, 'r') as my_file:
            stats_sentence = my_file.readline().strip()
            attributes_sentence = my_file.readline().strip()
            line = my_file.readline().strip()
            while(line):
                rules.append(line)
                line = my_file.readline().strip()
            my_file.close()

    except (FileNotFoundError):
        raise ValueError(f"Error : File {rules_file} not found.")
    except (IOError):
        raise ValueError(f"Error : Couldn't open file {rules_file}.")

    try:
        with open(output_file, 'w') as my_file:
            my_file.write(stats_sentence+"\n")
            my_file.write(attributes_sentence+"\n\n")
            for rule in rules:
                rule, stats = rule.split(" Covering size : ")
                rule1, _, rule3 = rule.split(":")
                rule4 = rule1+":"+rule3
                my_file.write(rule4+"\n")
                cov, stats = stats.split(" Fidelity : ")
                my_file.write(f"Train Covering size : {cov}\n")
                fid, stats = stats.split(" Accuracy : ")
                my_file.write(f"Train Fidelity : {fid}\n")
                acc, conf = stats.split(" Confidence : ")
                my_file.write(f"Train Accuracy : {acc}\n")
                my_file.write(f"Train Confidence : {conf}\n\n\n")

            my_file.close()

    except (FileNotFoundError):
        raise ValueError(f"Error : File {rules_file} not found.")
    except (IOError):
        raise ValueError(f"Error : Couldn't open file {rules_file}.")

    return 0

rulesConversion("datafiles/Mnist/withValid/globalRules.txt", "datafiles/Mnist/withValid/globalRulesFinal.rls")
