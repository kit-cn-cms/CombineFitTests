import ROOT

def getLatex(x,y,text):
    tests = ROOT.TLatex(x, y,text)
    tests.SetTextFont(42)
    tests.SetTextSize(0.04)
    tests.SetNDC()
    return tests


def getCanvas(name='c',ratiopad=False):
    c=ROOT.TCanvas(name,name,1024,1024)
    c.SetRightMargin(0.14)
    c.SetTopMargin(0.12)
    c.SetLeftMargin(0.12)
    c.SetBottomMargin(0.12)
    c.SetTicks(1,1)

    return c
def getLegend():
    legend=ROOT.TLegend()
    legend.SetX1NDC(0.15)
    legend.SetX2NDC(0.45)
    legend.SetY1NDC(0.68)
    legend.SetY2NDC(0.86)
    legend.SetBorderSize(0);
    legend.SetLineStyle(0);
    legend.SetTextFont(42);
    legend.SetTextSize(0.04);
    legend.SetFillStyle(1001); #1001 -> solid, 0 -> hollow
    return legend


def setupPad(p):
    p.SetRightMargin(0.14)
    p.SetTopMargin(0.12)
    p.SetLeftMargin(0.12)
    p.SetBottomMargin(0.12)
    p.SetTicks(1,1)


def lnn(beta,err):
    return pow(1.+err,beta)
    
def insert_values(cmds, keyword, toinsert, joinwith=","):
    if keyword in cmds:
        print "keyword %s already exists! Inserting..." % keyword
        i = cmds.index(keyword)
        cmds[i+1] = joinwith.join([cmds[i+1],toinsert])
    else:
        cmds += [keyword, toinsert]