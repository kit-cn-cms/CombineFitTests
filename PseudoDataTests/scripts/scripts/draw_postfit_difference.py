import os
import sys
import ROOT

thispath = os.path.realpath(os.path.abspath(sys.argv[0]))
directory = os.path.dirname(thispath)
basefolder = os.path.abspath(os.path.join(directory, "base"))

if not basefolder in sys.path:
    sys.path.append(basefolder)

from helperClass import helperClass

def check_file(path):
    print "checking path", path
    if not os.path.exists(path):
        sys.exit("ERROR: path '%s' does not exist" % path)

    if not os.path.isfile(path):
        sys.exit("ERROR: path '%s' is not a file" % path)

def parse_arguments():
    usage = """
    usage: %prog [options] path/to/fitresult_1 path/to/fitresult_2
    """

    parser = OptionParser(usage=usage)

    parser.add_option(  "-o", "--outname",
                        help="""name of the output file
                            """,
                        type = "str",
                        dest = "outname",
                        default = "np_difference"
                    )
    parser.add_option(  "-f" , "--fit_result",
                        help = "fit result to load (either 'fit_b' or 'fit_s'), default: fit_s",
                        choices = ["fit_b", "fit_s"],
                        dest = "fit_res_to_load",
                        default = "fit_s",
                        type = "str"
                    )
    

    (options, args) = parser.parse_args()
    return options, args

def load_results(resultpath_1, resultpath_2, fit_res_to_load):
    rfile_1 = ROOT.TFile.Open(resultpath_1)
    if not helper.intact_root_file(rfile_1):
        sys.exit(1)

    rfile_2 = ROOT.TFile.Open(resultpath_2)
    if not helper.intact_root_file(rfile2):
        sys.exit(1)
    fit_res_1 = helper.load_roofitresult(rfile = rfile_1, fitres=fit_res_to_load)
    fit_res_2 = helper.load_roofitresult(rfile = rfile_2, fitres = fit_res_to_load)

    variables = fit_res_1.floatParsFinal().contentsString().split(",")
    results = {}

    for v in variables:
        if "Bin" in v or "prop_bin" in v:
            continue
        v1 = helper.load_variable(result = fit_res_1, parname = v)
        v2 = helper.load_variable(result = fit_res_2, parname = v)

        if v1 is None:
            print "ERROR: Could not load variable '%s' from '%s'" % (v, resultpath_1)
            continue
        if v2 is None:
            print "ERROR: Could not load variable '%s' from '%s'" % (v, resultpath_1)
            continue

        results[v] = v1.getVal() - v2.getVal()
    return results

def construct_graph(results, outname):
    pass

def main(options, filepaths):
    resultpath_1 = filepaths[0]
    resultpath_2 = filepaths[1]

    check_file(resultpath_1)
    check_file(resultpath_2)

    fit_res_to_load = options.fit_res_to_load

    helper = helperClass()

    results = load_results(resultpath_1, resultpath_2, fit_res_to_load)

    print "loaded %s variables" % str(len(results))

    construct_graph(results = results, outname = outname)

if __name__ == '__main__':
    options, filepaths = parse_arguments()
    main(options = options, filepaths = filepaths)