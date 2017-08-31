.. _CTAMacros:

=====================================================
#Macros to use MARS to analyse CTA simulations
=====================================================

These macros are the result of the work of (mainly) Abelardo Moralejo, Tarek Hassan and Paolo Cumani. Please acknowledge their contribution.

ATTENTION: BE SURE NOT TO USE THESE MACROS AS THEY ARE (ESPECIALLY THE ONES RELATED TO THE USE OF GRID). YOU HAVE TO CHANGE AT LEAST THE PATH WHERE THE OUTPUT FILES ARE SAVED.

For sure you will have to substitute /vo.cta.in2p3.fr/user/p/pcumani with /vo.cta.in2p3.fr/user/<initial of your grid username>/<your grid username> (plus some other stuff very likely)

Three directories are present:

1. 3HB9: It contains all the macros that were used to analyse the Prod3b production for Paranal, from the training to the IRFs computation.
Again: be sure to change the output folder were you saved the results (otherwise you're going to overwrite my files :P).

2. Installing: Macros for installing a custom of version of ROOT, MARS or Chimp on the Grid, to be used by the job launched later
Again: be sure to change the output folder were you saved the results (otherwise you're going to overwrite my files :P).

3. Plotting: Contains macros to plot layouts from a Star or Stereo file (CTAmap_layouts), visualize one or more events from Chimp files produced without the startoutput option
(CTAplotEvent) or a summary plot of all the IRFs from the sensitivity created by CTAsensi (CTAplot_Summary).
