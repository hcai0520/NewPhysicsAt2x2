import ROOT
import numpy as np

# Open ROOT files
mu = ROOT.TFile.Open("mu+.root")
mcp = ROOT.TFile.Open("mcp+.root")

# Get the TTrees
mu_ntuple = mu.Get("ntuple")  
mcp_ntuple = mcp.Get("ntuple")
print('1')


x_mu   = np.array([])
y_mu   = np.array([])
z_mu   = np.array([])
evt_mu = np.array([])
e_mu   = np.array([])

x_mcp   = np.array([])
y_mcp   = np.array([])
z_mcp   = np.array([])
evt_mcp = np.array([])
e_mcp   = np.array([])

for entry in mu_ntuple:
  x_mu   = np.append(x_mu, entry.x)
  y_mu   = np.append(y_mu, entry.y)
  z_mu   = np.append(z_mu, entry.z)
  evt_mu = np.append(evt_mu,entry.evt)
  e_mu   = np.append(e_mu,entry.energy)
print(3)
for entry in mcp_ntuple:
  x_mcp   = np.append(x_mcp, entry.x)
  y_mcp   = np.append(y_mcp, entry.y)
  z_mcp   = np.append(z_mcp, entry.z)
  evt_mcp = np.append(evt_mcp,entry.evt)
  e_mcp   = np.append(e_mcp,entry.energy)
print('2')
EOverR_mu = np.array([])

for i in np.unique(evt_mu):
  x_mu_i = x_mu[evt_mu == i]  
  y_mu_i = y_mu[evt_mu == i]
  z_mu_i = z_mu[evt_mu == i] 
  e_mu_i = e_mu[evt_mu == i]
  for j in range(len(x_mu_i)):
    if j == 0:
      EOverR_mu = np.append(EOverR_mu,e_mu_i[j]/np.sqrt(x_mu_i[j]**2 + y_mu_i[j]**2 + z_mu_i[j]**2) )
    else:
      EOverR_mu = np.append(EOverR_mu,e_mu_i[j]/np.sqrt((x_mu_i[j]-x_mu_i[j-1])**2 + (y_mu_i[j]-y_mu_i[j-1])**2 + (z_mu_i[j]-z_mu_i[j-1])**2) )
print(len(EOverR_mu), len(z_mu))

EOverR_mcp = np.array([])

for i in np.unique(evt_mcp):
  x_mcp_i = x_mcp[evt_mcp == i]  
  y_mcp_i = y_mcp[evt_mcp == i]
  z_mcp_i = z_mcp[evt_mcp == i] 
  e_mcp_i = e_mcp[evt_mcp == i]
  for j in range(len(x_mcp_i)):
    if j == 0:
      EOverR_mcp = np.append(EOverR_mcp,e_mcp_i[j]/np.sqrt(x_mcp_i[j]**2 + y_mcp_i[j]**2 + z_mcp_i[j]**2) )
    else:
      EOverR_mcp = np.append(EOverR_mcp,e_mcp_i[j]/np.sqrt((x_mcp_i[j]-x_mcp_i[j-1])**2 + (y_mcp_i[j]-y_mcp_i[j-1])**2 + (z_mcp_i[j]-z_mcp_i[j-1])**2) )
print(len(EOverR_mcp), len(z_mcp))

prof1 = ROOT.TProfile("mu+",  "dE/dr vs z;z;Mean dE/dr", 100, 0, 500)
prof2 = ROOT.TProfile("mcp+", "dE/dr vs z;z;Mean dE/dr", 100, 0, 500)

for z, e in zip(z_mu, EOverR_mu):
  prof1.Fill(z, e)

for z, e in zip(z_mcp, EOverR_mcp):
  prof2.Fill(z, e)




# Customize histograms
prof1.SetLineColor(ROOT.kBlue)
prof1.SetLineWidth(2)
prof2.SetLineColor(ROOT.kRed)
prof2.SetLineWidth(2)

# Canvas to draw
c = ROOT.TCanvas("c", "dE/dz Comparison", 800, 600)
prof1.SetTitle("dE/dz vs z;z [cm];dE/dz [MeV/cm]")
prof1.Draw()
prof2.Draw("SAME")

# Add a legend
legend = ROOT.TLegend(0.7, 0.75, 0.9, 0.9)
legend.AddEntry(prof1, "Mu+", "l")
legend.AddEntry(prof2, "MCP+", "l")
legend.Draw()

# Save or show
c.SaveAs("dedz_comparison.png")
