/*******************************************************************************
 *  
 * This file is part of iBioSim. Please visit <http://www.async.ece.utah.edu/ibiosim>
 * for the latest version of iBioSim.
 *
 * Copyright (C) 2017 University of Utah
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the Apache License. A copy of the license agreement is provided
 * in the file named "LICENSE.txt" included with this software distribution
 * and also available online at <http://www.async.ece.utah.edu/ibiosim/License>.
 *  
 *******************************************************************************/
package edu.utah.ece.async.ibiosim.gui;

import java.awt.AWTError;
import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Font;
import java.awt.KeyEventDispatcher;
import java.awt.KeyboardFocusManager;
import java.awt.Point;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.Observable;
import java.util.Observer;
import java.util.Properties;
import java.util.Set;
import java.util.prefs.Preferences;

import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JSplitPane;
import javax.swing.JTabbedPane;
import javax.swing.JTextArea;
import javax.swing.JToolBar;
import javax.swing.JTree;
import javax.swing.JViewport;
import javax.swing.KeyStroke;
import javax.swing.SwingConstants;
import javax.swing.SwingUtilities;
import javax.swing.WindowConstants;

import org.antlr.runtime.ANTLRFileStream;
import org.antlr.runtime.CommonTokenStream;
import org.antlr.runtime.RecognitionException;
import org.antlr.runtime.TokenStream;
import org.jlibsedml.SEDMLDocument;

import edu.utah.ece.async.ibiosim.dataModels.util.Executables;
import edu.utah.ece.async.ibiosim.dataModels.util.GlobalConstants;
import edu.utah.ece.async.ibiosim.dataModels.util.IBioSimPreferences;
import edu.utah.ece.async.ibiosim.dataModels.util.exceptions.BioSimException;
import edu.utah.ece.async.ibiosim.gui.analysisView.AnalysisView;
import edu.utah.ece.async.ibiosim.gui.graphEditor.Graph;
import edu.utah.ece.async.ibiosim.gui.learnView.DataManager;
import edu.utah.ece.async.ibiosim.gui.learnView.LearnView;
import edu.utah.ece.async.ibiosim.gui.lpnEditor.LHPNEditor;
import edu.utah.ece.async.ibiosim.gui.modelEditor.movie.MovieContainer;
import edu.utah.ece.async.ibiosim.gui.modelEditor.schematic.ModelEditor;
import edu.utah.ece.async.ibiosim.gui.synthesisView.SynthesisView;
import edu.utah.ece.async.ibiosim.gui.synthesisView.SynthesisViewATACS;
import edu.utah.ece.async.ibiosim.gui.util.FileTree;
import edu.utah.ece.async.ibiosim.gui.util.Log;
import edu.utah.ece.async.ibiosim.gui.util.Utility;
import edu.utah.ece.async.ibiosim.gui.util.preferences.EditPreferences;
import edu.utah.ece.async.ibiosim.gui.util.preferences.PreferencesDialog;
import edu.utah.ece.async.ibiosim.gui.util.tabs.CloseAndMaxTabbedPane;
//import edu.utah.ece.async.ibiosim.gui.verificationView.AbstractionPanel;
import edu.utah.ece.async.ibiosim.gui.verificationView.VerificationView;
import edu.utah.ece.async.lema.verification.lpn.LPN;
import edu.utah.ece.async.lema.verification.lpn.Lpn2verilog;
import edu.utah.ece.async.lema.verification.lpn.Translator;
import edu.utah.ece.async.lema.verification.lpn.properties.BuildProperty;
import edu.utah.ece.async.lema.verification.platu.platuLpn.io.PlatuGrammarLexer;
import edu.utah.ece.async.lema.verification.platu.platuLpn.io.PlatuGrammarParser;

/**
 * This class creates a GUI for the Tstubd program. It implements the
 * ActionListener class. This allows the GUI to perform actions when menu items
 * are selected.
 * 
 * @author Curtis Madsen
 * @author Tramy Nguyen
 * @author Chris Myers
 * @author <a href="http://www.async.ece.utah.edu/ibiosim#Credits"> iBioSim
 *         Contributors </a>
 * @version %I%
 */
public class atacsGui extends Gui implements Observer, MouseListener, ActionListener, MouseMotionListener, MouseWheelListener {

	private static final String atacsVersion = "6.1";
	private JMenuItem newVhdl, newLhpn, newG, newCsp, newHse, newUnc, newRsg;
	private JMenuItem importG, importCsp, importHse, importUnc, importVhdl, importRsg;
	
	protected JMenuItem viewCircuit, viewRules, viewTrace, viewLog,	saveAsVerilog, viewModGraph;

	/**
	 * This is the constructor for the Proj class. It initializes all the input
	 * fields, puts them on panels, adds the panels to the frame, and then
	 * displays the frame.
	 * 
	 * @throws Exception
	 */
	public atacsGui() {
		super();
		Thread.setDefaultUncaughtExceptionHandler(new Utility.UncaughtExceptionHandler());
		ENVVAR = System.getenv("ATACSGUI");
		System.setProperty("software.running", "ATACS Version " + atacsVersion);

		// Creates a new frame
		frame = new JFrame("ATACS");
		frame.setIconImage(ResourceManager.getImageIcon("ATACS.png").getImage());

		// Makes it so that clicking the x in the corner closes the program
		WindowListener w = new WindowListener() {
			@Override
			public void windowClosing(WindowEvent arg0) {
				exit.doClick();
			}

			@Override
			public void windowOpened(WindowEvent arg0) {
			}

			@Override
			public void windowClosed(WindowEvent arg0) {
			}

			@Override
			public void windowIconified(WindowEvent arg0) {
			}

			@Override
			public void windowDeiconified(WindowEvent arg0) {
			}

			@Override
			public void windowActivated(WindowEvent arg0) {
			}

			@Override
			public void windowDeactivated(WindowEvent arg0) {
			}
		};
		frame.addWindowListener(w);
		frame.setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
		popup = new JPopupMenu();
		popup.addMouseListener(this);

		// Sets up the Tool Bar
		toolbar = new JToolBar();
		String imgName = "save.png";
		saveButton = makeToolButton(imgName, "save", "Save");
		toolbar.add(saveButton);
		imgName = "saveas.png";
		saveasButton = makeToolButton(imgName, "saveas", "Save As");
		toolbar.add(saveasButton);
		imgName = "savecheck.png";
		checkButton = makeToolButton(imgName, "check", "Save and Check");
		toolbar.add(checkButton);
		imgName = "export.jpg";
		exportButton = makeToolButton(imgName, "export", "Export");
		toolbar.add(exportButton);
		imgName = "run-icon.jpg";
		runButton = makeToolButton(imgName, "run", "Save and Run");
		toolbar.add(runButton);
		imgName = "refresh.jpg";
		refreshButton = makeToolButton(imgName, "refresh", "Refresh");
		toolbar.add(refreshButton);
		saveButton.setEnabled(false);
		runButton.setEnabled(false);
		refreshButton.setEnabled(false);
		saveasButton.setEnabled(false);
		checkButton.setEnabled(false);
		exportButton.setEnabled(false);

		// Creates a menu for the frame
		menuBar = new JMenuBar();
		file = new JMenu("File");
		help = new JMenu("Help");
		edit = new JMenu("Edit");
		openRecent = new JMenu("Open Recent");
		importMenu = new JMenu("Import");
		exportMenu = new JMenu("Export");
		exportDataMenu = new JMenu("Data");
		exportImageMenu = new JMenu("Image");
		exportMovieMenu = new JMenu("Movie");
		newMenu = new JMenu("New");
		view = new JMenu("View");
		viewModel = new JMenu("Model");
		tools = new JMenu("Tools");
		menuBar.add(file);
		menuBar.add(edit);
		menuBar.add(tools);
		menuBar.add(help);
		select = new JMenuItem("Select Mode");
		cut = new JMenuItem("Delete");
		addModule = new JMenuItem("Add Module");
		addVariable = new JMenuItem("Add Variable");
		addBoolean = new JMenuItem("Add Boolean");
		addPlace = new JMenuItem("Add Place");
		addTransition = new JMenuItem("Add Transition");
		addRule = new JMenuItem("Add Rule");
		addConstraint = new JMenuItem("Add Constraint");
		moveLeft = new JMenuItem("Move Left");
		moveRight = new JMenuItem("Move Right");
		moveUp = new JMenuItem("Move Up");
		moveDown = new JMenuItem("Move Down");
		undo = new JMenuItem("Undo");
		redo = new JMenuItem("Redo");
		copy = new JMenuItem("Copy File");
		rename = new JMenuItem("Rename File");
		delete = new JMenuItem("Delete File");
		manual = new JMenuItem("Manual");
		bugReport = new JMenuItem("Submit Bug Report");
		about = new JMenuItem("About");
		openProj = new JMenuItem("Open Project");
		clearRecent = new JMenuItem("Clear Recent");
		close = new JMenuItem("Close");
		closeAll = new JMenuItem("Close All");
		saveAll = new JMenuItem("Save All");
		pref = new JMenuItem("Preferences");
		newProj = new JMenuItem("Project");
		newSBMLModel = new JMenuItem("Model");
		newVhdl = new JMenuItem("VHDL Model");
		newLhpn = new JMenuItem("LPN Model");
		newG = new JMenuItem("Petri Net");
		newCsp = new JMenuItem("CSP Model");
		newHse = new JMenuItem("Handshaking Expansion");
		newUnc = new JMenuItem("Extended Burst Mode Machine");
		newRsg = new JMenuItem("Reduced State Graph");
		graph = new JMenuItem("TSD Graph");
		probGraph = new JMenuItem("Histogram");
		importSedml = new JMenuItem("SED-ML File");
		importSbml = new JMenuItem("SBML Model");
		importG = new JMenuItem("Petri Net");
		importLpn = new JMenuItem("LPN Model");
		importVhdl = new JMenuItem("VHDL Model");
		importCsp = new JMenuItem("CSP Model");
		importHse = new JMenuItem("Handshaking Expansion");
		importUnc = new JMenuItem("Extended Burst Mode Machine");
		importRsg = new JMenuItem("Reduced State Graph");
		exportSBML = new JMenuItem("SBML");
		exportFlatSBML = new JMenuItem("Flat SBML");
		exportArchive = new JMenuItem("Archive");
		exportCsv = new JMenuItem("CSV");
		exportDat = new JMenuItem("DAT");
		exportEps = new JMenuItem("EPS");
		exportJpg = new JMenuItem("JPG");
		exportPdf = new JMenuItem("PDF");
		exportPng = new JMenuItem("PNG");
		exportSvg = new JMenuItem("SVG");
		exportTsd = new JMenuItem("TSD");
		exportAvi = new JMenuItem("AVI");
		exportMp4 = new JMenuItem("MP4");
		save = new JMenuItem("Save");
		saveModel = new JMenuItem("Save Learned LPN");
		saveAsVerilog = new JMenuItem("Save as Verilog");
		saveAsVerilog.addActionListener(this);
		saveAsVerilog.setActionCommand("saveAsVerilog");
		saveAsVerilog.setEnabled(false);
		saveAs = new JMenuItem("Save As");
		run = new JMenuItem("Save and Run");
		check = new JMenuItem("Save and Check");
		refresh = new JMenuItem("Refresh");
		viewCircuit = new JMenuItem("Circuit");
		viewRules = new JMenuItem("Production Rules");
		viewTrace = new JMenuItem("Error Trace");
		viewLog = new JMenuItem("Log");
		viewModGraph = new JMenuItem("Model");
		createAnal = new JMenuItem("Analysis Tool");
		createLearn = new JMenuItem("Learn Tool");
		createSbml = new JMenuItem("Create SBML File");
		createSynth = new JMenuItem("Synthesis Tool");
		createVer = new JMenuItem("Verification Tool");
		exit = new JMenuItem("Exit");
		select.addActionListener(this);
		cut.addActionListener(this);
		addModule.addActionListener(this);
		addVariable.addActionListener(this);
		addBoolean.addActionListener(this);
		addPlace.addActionListener(this);
		addTransition.addActionListener(this);
		addRule.addActionListener(this);
		addConstraint.addActionListener(this);
		moveLeft.addActionListener(this);
		moveRight.addActionListener(this);
		moveUp.addActionListener(this);
		moveDown.addActionListener(this);
		undo.addActionListener(this);
		redo.addActionListener(this);
		copy.addActionListener(this);
		rename.addActionListener(this);
		delete.addActionListener(this);
		openProj.addActionListener(this);
		clearRecent.addActionListener(this);
		close.addActionListener(this);
		closeAll.addActionListener(this);
		saveAll.addActionListener(this);
		pref.addActionListener(this);
		manual.addActionListener(this);
		bugReport.addActionListener(this);
		newProj.addActionListener(this);
		newSBMLModel.addActionListener(this);
		newVhdl.addActionListener(this);
		newLhpn.addActionListener(this);
		newG.addActionListener(this);
		newCsp.addActionListener(this);
		newHse.addActionListener(this);
		newUnc.addActionListener(this);
		newRsg.addActionListener(this);
		exit.addActionListener(this);
		about.addActionListener(this);
		importSedml.addActionListener(this);
		importSbml.addActionListener(this);
		importVhdl.addActionListener(this);
		importLpn.addActionListener(this);
		importG.addActionListener(this);
		importCsp.addActionListener(this);
		importHse.addActionListener(this);
		importUnc.addActionListener(this);
		importRsg.addActionListener(this);
		exportSBML.addActionListener(this);
		exportFlatSBML.addActionListener(this);
		exportArchive.addActionListener(this);
		exportCsv.addActionListener(this);
		exportDat.addActionListener(this);
		exportEps.addActionListener(this);
		exportJpg.addActionListener(this);
		exportPdf.addActionListener(this);
		exportPng.addActionListener(this);
		exportSvg.addActionListener(this);
		exportTsd.addActionListener(this);
		exportAvi.addActionListener(this);
		exportMp4.addActionListener(this);
		graph.addActionListener(this);
		probGraph.addActionListener(this);
		save.addActionListener(this);
		saveAs.addActionListener(this);
		run.addActionListener(this);
		check.addActionListener(this);
		refresh.addActionListener(this);
		saveModel.addActionListener(this);
		viewCircuit.addActionListener(this);
		viewRules.addActionListener(this);
		viewTrace.addActionListener(this);
		viewLog.addActionListener(this);
		viewModGraph.addActionListener(this);
		createAnal.addActionListener(this);
		createLearn.addActionListener(this);
		createSbml.addActionListener(this);
		createSynth.addActionListener(this);
		createVer.addActionListener(this);
		save.setActionCommand("save");
		saveAs.setActionCommand("saveas");

		run.setActionCommand("run");
		check.setActionCommand("check");
		refresh.setActionCommand("refresh");
		viewModGraph.setActionCommand("viewModel");
		createLearn.setActionCommand("createLearn");
		createSbml.setActionCommand("createSBML");
		createVer.setActionCommand("createVerify");

		ShortCutKey = Toolkit.getDefaultToolkit().getMenuShortcutKeyMask();
		newProj.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_P, ShortCutKey));
		openProj.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, ShortCutKey));
		close.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_W, ShortCutKey));
		closeAll.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_W, ShortCutKey | InputEvent.SHIFT_MASK));
		saveAll.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, ShortCutKey | InputEvent.ALT_MASK));
		save.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, ShortCutKey));
		saveAs.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, ShortCutKey | InputEvent.SHIFT_MASK));
		run.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_R, ShortCutKey));
		check.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_K, ShortCutKey));
		refresh.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_F5, 0));
		newSBMLModel.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_M, ShortCutKey));
		createAnal.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_A, ShortCutKey | InputEvent.SHIFT_MASK));
		createSynth.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Y, ShortCutKey | InputEvent.SHIFT_MASK));
		createLearn.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_L, ShortCutKey | InputEvent.SHIFT_MASK));
		newLhpn.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_L, ShortCutKey));
		graph.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_G, ShortCutKey));
		probGraph.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_H, ShortCutKey | InputEvent.SHIFT_MASK));
		select.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0));
		cut.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_BACK_SPACE, 0));
		addModule.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_M, 0));
		addVariable.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_V, 0));
		addBoolean.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_B, 0));
		addPlace.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_P, 0));
		addTransition.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_T, 0));
		addRule.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_R, InputEvent.SHIFT_MASK));
		addConstraint.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_C, InputEvent.SHIFT_MASK));
		moveLeft.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_LEFT, InputEvent.SHIFT_MASK));
		moveRight.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_RIGHT, InputEvent.SHIFT_MASK));
		moveUp.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_UP, InputEvent.SHIFT_MASK));
		moveDown.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_DOWN, InputEvent.SHIFT_MASK));
		undo.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Z, ShortCutKey));
		redo.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Z, ShortCutKey | InputEvent.SHIFT_MASK));
		copy.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_C, ShortCutKey | InputEvent.SHIFT_MASK));
		rename.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_V, ShortCutKey | InputEvent.SHIFT_MASK));
		delete.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_X, ShortCutKey | InputEvent.SHIFT_MASK));
		manual.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_M, ShortCutKey | InputEvent.SHIFT_MASK));
		bugReport.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_B, ShortCutKey));
		exit.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_X, ShortCutKey));
		pref.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_COMMA, ShortCutKey));

		importSedml.setEnabled(false);
		importSbml.setEnabled(false);
		importVhdl.setEnabled(false);
		importLpn.setEnabled(false);
		importG.setEnabled(false);
		importCsp.setEnabled(false);
		importHse.setEnabled(false);
		importUnc.setEnabled(false);
		importRsg.setEnabled(false);
		exportMenu.setEnabled(false);
		exportSBML.setEnabled(false);
		exportFlatSBML.setEnabled(false);
		exportArchive.setEnabled(false);
		exportCsv.setEnabled(false);
		exportDat.setEnabled(false);
		exportEps.setEnabled(false);
		exportJpg.setEnabled(false);
		exportPdf.setEnabled(false);
		exportPng.setEnabled(false);
		exportSvg.setEnabled(false);
		exportTsd.setEnabled(false);
		exportAvi.setEnabled(false);
		exportMp4.setEnabled(false);
		newSBMLModel.setEnabled(false);
		newVhdl.setEnabled(false);
		newLhpn.setEnabled(false);
		newG.setEnabled(false);
		newCsp.setEnabled(false);
		newHse.setEnabled(false);
		newUnc.setEnabled(false);
		newRsg.setEnabled(false);
		graph.setEnabled(false);
		probGraph.setEnabled(false);
		save.setEnabled(false);
		saveModel.setEnabled(false);
		saveAs.setEnabled(false);
		saveAll.setEnabled(false);
		close.setEnabled(false);
		closeAll.setEnabled(false);
		importMenu.setEnabled(false);

		run.setEnabled(false);
		check.setEnabled(false);
		refresh.setEnabled(false);
		cut.setEnabled(false);
		select.setEnabled(false);
		addModule.setEnabled(false);
		addVariable.setEnabled(false);
		addBoolean.setEnabled(false);
		addPlace.setEnabled(false);
		addTransition.setEnabled(false);
		addRule.setEnabled(false);
		addConstraint.setEnabled(false);
		moveLeft.setEnabled(false);
		moveRight.setEnabled(false);
		moveUp.setEnabled(false);
		moveDown.setEnabled(false);
		undo.setEnabled(false);
		redo.setEnabled(false);
		copy.setEnabled(false);
		rename.setEnabled(false);
		delete.setEnabled(false);
		viewCircuit.setEnabled(false);
		viewRules.setEnabled(false);
		viewTrace.setEnabled(false);
		viewLog.setEnabled(false);
		viewModel.setEnabled(false);
		viewModGraph.setEnabled(false);
		createAnal.setEnabled(false);
		createLearn.setEnabled(false);
		createSbml.setEnabled(false);
		createSynth.setEnabled(false);
		createVer.setEnabled(false);
		edit.add(undo);
		edit.add(redo);
		edit.addSeparator();
		edit.add(select);
		edit.add(cut);
		edit.add(moveLeft);
		edit.add(moveRight);
		edit.add(moveUp);
		edit.add(moveDown);
		edit.add(addModule);
		edit.add(addVariable);
		edit.add(addBoolean);
		edit.add(addPlace);
		edit.add(addTransition);
		edit.add(addRule);
		edit.add(addConstraint);
		edit.addSeparator();
		edit.add(copy);
		edit.add(rename);
		edit.add(delete);
		file.add(newMenu);
		newMenu.add(newProj);
		newMenu.add(graph);
		newMenu.add(probGraph);
		newMenu.add(newVhdl);
		newMenu.add(newG);
		newMenu.add(newLhpn);
		newMenu.add(newCsp);
		newMenu.add(newHse);
		newMenu.add(newUnc);
		newMenu.add(newRsg);
		file.add(openProj);
		file.add(openRecent);
		file.addSeparator();
		file.add(close);
		file.add(closeAll);
		file.addSeparator();
		file.add(save);
		file.add(run);
		file.add(saveAs);
		file.add(saveAll);
		file.addSeparator();
		file.add(importMenu);
		importMenu.add(importVhdl);
		importMenu.add(importG);
		importMenu.add(importLpn);
		importMenu.add(importCsp);
		importMenu.add(importHse);
		importMenu.add(importUnc);
		importMenu.add(importRsg);
		file.add(exportMenu);
		exportMenu.add(exportDataMenu);
		exportMenu.add(exportImageMenu);
		exportMenu.add(exportMovieMenu);
		exportMenu.add(exportFlatSBML);
		exportMenu.add(exportSBML);
		exportMenu.add(exportArchive);

		exportDataMenu.add(exportTsd);
		exportDataMenu.add(exportCsv);
		exportDataMenu.add(exportDat);
		exportImageMenu.add(exportEps);
		exportImageMenu.add(exportJpg);
		exportImageMenu.add(exportPdf);
		exportImageMenu.add(exportPng);
		exportImageMenu.add(exportSvg);
		exportMovieMenu.add(exportAvi);
		exportMovieMenu.add(exportMp4);
		help.add(manual);
		help.add(bugReport);
		if (System.getProperty("os.name").toLowerCase().startsWith("mac os")) {
			OSXSetup();
		} else {
			edit.addSeparator();
			edit.add(pref);
			file.add(exit);
			// file.addSeparator();
			help.add(about);
		}
		menuBar.add(view);
		view.add(viewModGraph);
		view.add(viewCircuit);
		view.add(viewRules);
		view.add(viewTrace);
		view.add(viewLog);
		tools.add(createAnal);
		tools.add(createSynth);
		tools.add(createVer);
		root = null;

		// Create recent project menu items
		numberRecentProj = 0;
		recentProjects = new JMenuItem[10];
		recentProjectPaths = new String[10];
		for (int i = 0; i < 10; i++) {
			recentProjects[i] = new JMenuItem();
			recentProjects[i].addActionListener(this);
			recentProjects[i].setActionCommand("recent" + i);
			recentProjectPaths[i] = "";
		}
		recentProjects[0].setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_0, ShortCutKey));
		recentProjects[0].setMnemonic(KeyEvent.VK_0);
		recentProjects[1].setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_1, ShortCutKey));
		recentProjects[1].setMnemonic(KeyEvent.VK_1);
		recentProjects[2].setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_2, ShortCutKey));
		recentProjects[2].setMnemonic(KeyEvent.VK_2);
		recentProjects[3].setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_3, ShortCutKey));
		recentProjects[3].setMnemonic(KeyEvent.VK_3);
		recentProjects[4].setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_4, ShortCutKey));
		recentProjects[4].setMnemonic(KeyEvent.VK_4);
		recentProjects[5].setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_5, ShortCutKey));
		recentProjects[5].setMnemonic(KeyEvent.VK_5);
		recentProjects[6].setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_6, ShortCutKey));
		recentProjects[6].setMnemonic(KeyEvent.VK_6);
		recentProjects[7].setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_7, ShortCutKey));
		recentProjects[7].setMnemonic(KeyEvent.VK_7);
		recentProjects[8].setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_8, ShortCutKey));
		recentProjects[8].setMnemonic(KeyEvent.VK_8);
		recentProjects[9].setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_9, ShortCutKey));
		recentProjects[9].setMnemonic(KeyEvent.VK_9);

		Preferences biosimrc = Preferences.userRoot();
		viewer = biosimrc.get("biosim.general.viewer", "");
		for (int i = 0; i < 10; i++) {
			recentProjects[i].setText(biosimrc.get("atacs.recent.project." + i, ""));
			recentProjectPaths[i] = biosimrc.get("atacs.recent.project.path." + i, "");
			if (!recentProjects[i].getText().trim().equals("") && !recentProjectPaths[i].trim().equals("")) {
				openRecent.add(recentProjects[i]);
				numberRecentProj = i + 1;
			} else {
				break;
			}
		}
		openRecent.addSeparator();
		openRecent.add(clearRecent);
		SBMLLevelVersion = "L3V2";

		// Packs the frame and displays it
		mainPanel = new JPanel(new BorderLayout());
		tree = new FileTree(null, this, false, true, false);

		EditPreferences editPreferences = new EditPreferences(frame, true);
		editPreferences.setDefaultPreferences();
		tree.setExpandibleIcons(!IBioSimPreferences.INSTANCE.isPlusMinusIconsEnabled());

		log = new Log();
		tab = new CloseAndMaxTabbedPane(false, this);
		tab.setPreferredSize(new Dimension(1100, 550));
		// tab.getPaneUI().addMouseListener(this);

		topSplit = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, tree, tab);
		mainSplit = new JSplitPane(JSplitPane.VERTICAL_SPLIT, topSplit, log);

		// mainPanel.add(tree, "West");
		mainPanel.add(mainSplit, "Center");
		// mainPanel.add(log, "South");
		mainPanel.add(toolbar, "North");
		frame.setContentPane(mainPanel);
		frame.setJMenuBar(menuBar);
		frame.addMouseListener(this);
		menuBar.addMouseListener(this);
		frame.pack();
		Dimension screenSize;
		try {
			Toolkit tk = Toolkit.getDefaultToolkit();
			screenSize = tk.getScreenSize();
		} catch (AWTError awe) {
			screenSize = new Dimension(640, 480);
		}
		Dimension frameSize = frame.getSize();

		if (frameSize.height > screenSize.height) {
			frameSize.height = screenSize.height;
			frame.setSize(frameSize);
		}
		if (frameSize.width > screenSize.width) {
			frameSize.width = screenSize.width;
			frame.setSize(frameSize);
		}
		int x = screenSize.width / 2 - frameSize.width / 2;
		int y = screenSize.height / 2 - frameSize.height / 2;
		frame.setLocation(x, y);
		frame.setVisible(true);
		dispatcher = new KeyEventDispatcher() {
			@Override
			public boolean dispatchKeyEvent(KeyEvent e) {
				if (e.getID() == KeyEvent.KEY_TYPED) {
					if (e.getKeyChar() == '') {
						if (tab.getTabCount() > 0) {
							KeyboardFocusManager.getCurrentKeyboardFocusManager().removeKeyEventDispatcher(dispatcher);
							if (save(tab.getSelectedIndex(), 0) != 0) {
								tab.remove(tab.getSelectedIndex());
							}
							KeyboardFocusManager.getCurrentKeyboardFocusManager().addKeyEventDispatcher(dispatcher);
						}
					}
				}
				return false;
			}
		};
		KeyboardFocusManager.getCurrentKeyboardFocusManager().addKeyEventDispatcher(dispatcher);
	}

	// TODO: keeper
	private void about() {
		final JFrame f = new JFrame("About");
		JLabel name;
		JLabel version;
		final String developers;
		name = new JLabel("ATACS", SwingConstants.CENTER);
		version = new JLabel("Version " + atacsVersion, SwingConstants.CENTER);
		developers = "Wendy Belluomini\nJeff Cuthbert\nHans Jacobson\nKevin Jones\nSung-Tae Jung\n"
				+ "Christopher Krieger\nScott Little\nCurtis Madsen\nEric Mercer\nChris Myers\n"
				+ "Curt Nelson\nEric Peskin\nNicholas Seegmiller\nDavid Walter\nHao Zheng";
		Font font = name.getFont();
		font = font.deriveFont(Font.BOLD, 36.0f);
		name.setFont(font);
		JLabel uOfU = new JLabel("University of Utah", SwingConstants.CENTER);
		JButton credits = new JButton("Credits");
		credits.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				Object[] options = { "Close" };
				JOptionPane.showOptionDialog(f, developers, "Credits", JOptionPane.YES_OPTION,
						JOptionPane.PLAIN_MESSAGE, null, options, options[0]);
			}
		});
		JButton close = new JButton("Close");
		close.addActionListener(new ActionListener() {
			@Override
			public void actionPerformed(ActionEvent e) {
				f.dispose();
			}
		});
		JPanel buttons = new JPanel();
		buttons.add(credits);
		buttons.add(close);
		JPanel aboutPanel = new JPanel(new BorderLayout());
		JPanel uOfUPanel = new JPanel(new BorderLayout());
		uOfUPanel.add(name, "North");
		uOfUPanel.add(version, "Center");
		uOfUPanel.add(uOfU, "South");
		aboutPanel.add(new javax.swing.JLabel(ResourceManager.getImageIcon("ATACS.png")),"North");
		aboutPanel.add(uOfUPanel, "Center");
		aboutPanel.add(buttons, "South");
		f.setContentPane(aboutPanel);
		f.pack();
		Dimension screenSize;
		try {
			Toolkit tk = Toolkit.getDefaultToolkit();
			screenSize = tk.getScreenSize();
		} catch (AWTError awe) {
			screenSize = new Dimension(640, 480);
		}
		Dimension frameSize = f.getSize();

		if (frameSize.height > screenSize.height) {
			frameSize.height = screenSize.height;
		}
		if (frameSize.width > screenSize.width) {
			frameSize.width = screenSize.width;
		}
		int x = screenSize.width / 2 - frameSize.width / 2;
		int y = screenSize.height / 2 - frameSize.height / 2;
		f.setLocation(x, y);
		f.setVisible(true);
	}

	public boolean exit() {
		int autosave = 0;
		for (int i = 0; tab != null && i < tab.getTabCount(); i++) {
			int save = save(i, autosave);
			if (save == 0) {
				return false;
			} else if (save == 2) {
				autosave = 1;
			} else if (save == 3) {
				autosave = 2;
			}
		}
		Preferences biosimrc = Preferences.userRoot();
		for (int i = 0; i < numberRecentProj; i++) {
			biosimrc.put("atacs.recent.project." + i, recentProjects[i].getText());
			biosimrc.put("atacs.recent.project.path." + i, recentProjectPaths[i]);
		}
		for (int i = numberRecentProj; i < 10; i++) {
			biosimrc.put("atacs.recent.project." + i, "");
			biosimrc.put("atacs.recent.project.path." + i, "");
		}
		System.exit(1);
		return true;
	}

	public String getCurrentProjectId() {
		return currentProjectId;
	}

	private void createProject(ActionEvent e) {
		int autosave = 0;
		for (int i = 0; i < tab.getTabCount(); i++) {
			int save = save(i, autosave);
			if (save == 0) {
				return;
			} else if (save == 2) {
				autosave = 1;
			} else if (save == 3) {
				autosave = 2;
			}
		}
		File file;
		Preferences biosimrc = Preferences.userRoot();
		if (biosimrc.get("biosim.general.project_dir", "").equals("")) {
			file = null;
		} else {
			file = new File(biosimrc.get("biosim.general.project_dir", ""));
		}
		String filename;

		if (e.getActionCommand().startsWith(GlobalConstants.SBOL_SYNTH_COMMAND)) {
			filename = identifySBOLSynthesisPath(e.getActionCommand());
		} else {
			filename = Utility.browse(frame, file, null, JFileChooser.DIRECTORIES_ONLY, "New", -1);
		}
		if (!filename.trim().equals("")) {
			filename = filename.trim();
			biosimrc.put("biosim.general.project_dir", filename);
			File f = new File(filename);
			if (f.exists()) {
				Object[] options = { "Overwrite", "Cancel" };
				int value = JOptionPane.showOptionDialog(frame, "File already exists." + "\nDo you want to overwrite?",
						"Overwrite", JOptionPane.YES_NO_OPTION, JOptionPane.PLAIN_MESSAGE, null, options, options[0]);
				if (value == JOptionPane.YES_OPTION) {
					File dir = new File(filename);
					if (dir.isDirectory()) {
						deleteDir(dir);
					} else {
						System.gc();
						dir.delete();
					}
				} else {
					return;
				}
			}
			new File(filename).mkdir();
			try {
				new FileWriter(new File(filename + File.separator + "ATACS.prj")).close();
			} catch (IOException e1) {
				JOptionPane.showMessageDialog(frame, "Unable to create a new project.", "Error",
						JOptionPane.ERROR_MESSAGE);
				return;
			}
			root = filename;
			currentProjectId = GlobalConstants.getFilename(root);

			sedmlDocument = new SEDMLDocument(1, 2);
			writeSEDMLDocument();

			refresh(false,true,false);
			tab.removeAll();
			addRecentProject(filename);

			addToTree(currentProjectId + ".sbol");

			// importDot.setEnabled(true);
			importMenu.setEnabled(true);
			importSedml.setEnabled(true);
			importSbml.setEnabled(true);
			importVhdl.setEnabled(true);
			importLpn.setEnabled(true);
			importG.setEnabled(true);
			importCsp.setEnabled(true);
			importHse.setEnabled(true);
			importUnc.setEnabled(true);
			importRsg.setEnabled(true);
			newSBMLModel.setEnabled(true);
			newVhdl.setEnabled(true);
			newLhpn.setEnabled(true);
			newG.setEnabled(true);
			newCsp.setEnabled(true);
			newHse.setEnabled(true);
			newUnc.setEnabled(true);
			newRsg.setEnabled(true);
			graph.setEnabled(true);
			probGraph.setEnabled(true);
		}
	}

	private void openProject(ActionEvent e) {
		int autosave = 0;
		for (int i = 0; i < tab.getTabCount(); i++) {
			int save = save(i, autosave);
			if (save == 0) {
				return;
			} else if (save == 2) {
				autosave = 1;
			} else if (save == 3) {
				autosave = 2;
			}
		}
		Preferences biosimrc = Preferences.userRoot();
		String projDir = "";
		if (e.getSource() == openProj) {
			File file;
			if (biosimrc.get("biosim.general.project_dir", "").equals("")) {
				file = null;
			} else {
				file = new File(biosimrc.get("biosim.general.project_dir", ""));
			}
			projDir = Utility.browse(frame, file, null, JFileChooser.DIRECTORIES_ONLY, "Open", -1);
			if (projDir.endsWith(".prj")) {
				biosimrc.put("biosim.general.project_dir", projDir);
				String[] tempArray = GlobalConstants.splitPath(projDir);
				projDir = "";
				for (int i = 0; i < tempArray.length - 1; i++) {
					projDir = projDir + tempArray[i] + File.separator;
				}
			}
		} else if (e.getSource() == recentProjects[0]) {
			projDir = recentProjectPaths[0];
		} else if (e.getSource() == recentProjects[1]) {
			projDir = recentProjectPaths[1];
		} else if (e.getSource() == recentProjects[2]) {
			projDir = recentProjectPaths[2];
		} else if (e.getSource() == recentProjects[3]) {
			projDir = recentProjectPaths[3];
		} else if (e.getSource() == recentProjects[4]) {
			projDir = recentProjectPaths[4];
		} else if (e.getSource() == recentProjects[5]) {
			projDir = recentProjectPaths[5];
		} else if (e.getSource() == recentProjects[6]) {
			projDir = recentProjectPaths[6];
		} else if (e.getSource() == recentProjects[7]) {
			projDir = recentProjectPaths[7];
		} else if (e.getSource() == recentProjects[8]) {
			projDir = recentProjectPaths[8];
		} else if (e.getSource() == recentProjects[9]) {
			projDir = recentProjectPaths[9];
		}
		// log.addText(projDir);
		if (!projDir.equals("")) {
			biosimrc.put("biosim.general.project_dir", projDir);
			if (new File(projDir).isDirectory()) {
				boolean isProject = false;
				for (String temp : new File(projDir).list()) {
					if (temp.equals(".prj")) {
						isProject = true;
					}
					if (temp.equals("ATACS.prj")) {
						isProject = true;
					} 
				}
				if (isProject) {
					root = projDir;
					currentProjectId = GlobalConstants.getFilename(root);
					readSEDMLDocument();
					readSBOLDocument();
					refresh(false,true,false);
					addToTree(currentProjectId + ".sbol");
					tab.removeAll();
					addRecentProject(projDir);

					// importDot.setEnabled(true);
					importMenu.setEnabled(true);
					importSedml.setEnabled(true);
					importSbml.setEnabled(true);
					importVhdl.setEnabled(true);
					importLpn.setEnabled(true);
					importG.setEnabled(true);
					importCsp.setEnabled(true);
					importHse.setEnabled(true);
					importUnc.setEnabled(true);
					importRsg.setEnabled(true);
					newSBMLModel.setEnabled(true);
					newVhdl.setEnabled(true);
					newLhpn.setEnabled(true);
					newG.setEnabled(true);
					newCsp.setEnabled(true);
					newHse.setEnabled(true);
					newUnc.setEnabled(true);
					newRsg.setEnabled(true);
					graph.setEnabled(true);
					probGraph.setEnabled(true);
				} else {
					JOptionPane.showMessageDialog(frame, "You must select a valid project.", "Error",
							JOptionPane.ERROR_MESSAGE);
					removeRecentProject(projDir);
				}
			} else {
				JOptionPane.showMessageDialog(frame, "You must select a valid project.", "Error",
						JOptionPane.ERROR_MESSAGE);
				removeRecentProject(projDir);
			}
		}
	}

	/**
	 * This method performs different functions depending on what menu items are
	 * selected.
	 */
	@Override
	public void actionPerformed(ActionEvent e) {
		if (e.getSource() == viewCircuit) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof JTabbedPane) {
				Component component = ((JTabbedPane) comp).getSelectedComponent();
				if (component instanceof LearnView) {
					((LearnView) component).viewModel();
				} 
			} else if (comp instanceof LHPNEditor) {
				((LHPNEditor) comp).viewLhpn();
			} else if (comp instanceof JPanel) {
				Component[] array = ((JPanel) comp).getComponents();
				if (array[0] instanceof VerificationView) {
					((VerificationView) array[0]).viewCircuit();
				} else if (array[0] instanceof SynthesisViewATACS) {
					((SynthesisViewATACS) array[0]).viewCircuit();
				}
			}
		} else if (e.getSource() == viewLog) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof VerificationView) {
				((VerificationView) comp).viewLog();
			} else if (comp instanceof JPanel) {
				Component[] array = ((JPanel) comp).getComponents();
				if (array[0] instanceof SynthesisViewATACS) {
					((SynthesisViewATACS) array[0]).viewLog();
				}
			} else if (comp instanceof JTabbedPane) {
				for (int i = 0; i < ((JTabbedPane) comp).getTabCount(); i++) {
					Component component = ((JTabbedPane) comp).getComponent(i);
					if (component instanceof LearnView) {
						((LearnView) component).viewLog();
						return;
					}
				}
			}
		} else if (e.getSource() == saveModel) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof JTabbedPane) {
				for (Component component : ((JTabbedPane) comp).getComponents()) {
					if (component instanceof LearnView) {
						((LearnView) component).saveModel();
					}
				}
			}
		} else if (e.getSource() == saveAsVerilog) {
			try {
				Lpn2verilog.convert(tree.getFile());
			} catch (BioSimException e1) {
				JOptionPane.showMessageDialog(atacsGui.frame, e1.getMessage(), e1.getTitle(), JOptionPane.ERROR_MESSAGE);
				e1.printStackTrace();
			}
			String theFile = "";
			if (tree.getFile().lastIndexOf('/') >= 0) {
				theFile = tree.getFile().substring(tree.getFile().lastIndexOf('/') + 1);
			}
			if (tree.getFile().lastIndexOf('\\') >= 0) {
				theFile = tree.getFile().substring(tree.getFile().lastIndexOf('\\') + 1);
			}
			addToTree(theFile.replace(".lpn", ".sv"));
		} else if (e.getSource() == close && tab.getSelectedComponent() != null) {
			Component comp = tab.getSelectedComponent();
			Point point = comp.getLocation();
			tab.fireCloseTabEvent(
					new MouseEvent(comp, e.getID(), e.getWhen(), e.getModifiers(), point.x, point.y, 0, false),
					tab.getSelectedIndex());
		} else if (e.getSource() == saveAll) {
			int autosave = 0;
			for (int i = 0; i < tab.getTabCount(); i++) {
				int save = save(i, autosave);
				if (save == 0) {
					break;
				} else if (save == 2) {
					autosave = 1;
				} else if (save == 3) {
					autosave = 2;
				}
				markTabClean(i);
			}
		} else if (e.getSource() == closeAll) {
			while (tab.getSelectedComponent() != null) {
				int index = tab.getSelectedIndex();
				Component comp = tab.getComponent(index);
				Point point = comp.getLocation();
				tab.fireCloseTabEvent(
						new MouseEvent(comp, e.getID(), e.getWhen(), e.getModifiers(), point.x, point.y, 0, false),
						index);
			}
		} else if (e.getSource() == viewRules) {
			Component comp = tab.getSelectedComponent();
			Component[] array = ((JPanel) comp).getComponents();
			((SynthesisViewATACS) array[0]).viewRules();
		} else if (e.getSource() == viewTrace) {
			Component comp = tab.getSelectedComponent();
			if (comp.getName().equals("Verification")) {
				Component[] array = ((JPanel) comp).getComponents();
				((VerificationView) array[0]).viewTrace();
			} else if (comp.getName().equals("Synthesis")) {
				Component[] array = ((JPanel) comp).getComponents();
				((SynthesisViewATACS) array[0]).viewTrace();
			}
		} else if (e.getSource() == exportFlatSBML) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof ModelEditor) {
				((ModelEditor) comp).exportFlatSBML();
			}
		} else if (e.getSource() == exportSBML) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof ModelEditor) {
				((ModelEditor) comp).exportSBML();
			}
		} 
		else if (e.getSource() == exportCsv) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof Graph) {
				((Graph) comp).export(5);
			} else if (comp instanceof JTabbedPane) {
				((Graph) ((JTabbedPane) comp).getSelectedComponent()).export(5);
			}
		} else if (e.getSource() == exportDat) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof Graph) {
				((Graph) comp).export(6);
			} else if (comp instanceof JTabbedPane) {
				((Graph) ((JTabbedPane) comp).getSelectedComponent()).export();
			}
		} else if (e.getSource() == exportEps) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof Graph) {
				((Graph) comp).export(3);
			} else if (comp instanceof JTabbedPane) {
				((Graph) ((JTabbedPane) comp).getSelectedComponent()).export(3);
			}
		} else if (e.getSource() == exportJpg) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof Graph) {
				((Graph) comp).export(0);
			} else if (comp instanceof JTabbedPane) {

				if (((JTabbedPane) comp).getSelectedComponent().getName().equals("ModelViewMovie")) {
					((MovieContainer) ((JTabbedPane) comp).getSelectedComponent()).outputJPG(-1, false);
				} else {
					((Graph) ((JTabbedPane) comp).getSelectedComponent()).export(0);
				}
			} else if (comp instanceof ModelEditor) {
				((ModelEditor) comp).saveSchematic();
			}
		} else if (e.getSource() == exportPdf) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof Graph) {
				((Graph) comp).export(2);
			} else if (comp instanceof JTabbedPane) {
				((Graph) ((JTabbedPane) comp).getSelectedComponent()).export(2);
			}
		} else if (e.getSource() == exportPng) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof Graph) {
				((Graph) comp).export(1);
			} else if (comp instanceof JTabbedPane) {
				((Graph) ((JTabbedPane) comp).getSelectedComponent()).export(1);
			}
		} else if (e.getSource() == exportSvg) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof Graph) {
				((Graph) comp).export(4);
			} else if (comp instanceof JTabbedPane) {
				((Graph) ((JTabbedPane) comp).getSelectedComponent()).export(4);
			}
		} else if (e.getSource() == exportTsd) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof Graph) {
				((Graph) comp).export(7);
			} else if (comp instanceof JTabbedPane) {
				((Graph) ((JTabbedPane) comp).getSelectedComponent()).export(7);
			}
		} else if (e.getSource() == exportAvi) {

			Component comp = tab.getSelectedComponent();

			if (comp instanceof JTabbedPane) {

				((MovieContainer) ((JTabbedPane) comp).getSelectedComponent()).outputMovie("avi");
			}
		} else if (e.getSource() == exportMp4) {

			Component comp = tab.getSelectedComponent();

			if (comp instanceof JTabbedPane) {

				((MovieContainer) ((JTabbedPane) comp).getSelectedComponent()).outputMovie("mp4");
			}
		} else if (e.getSource() == about) {
			about();
		} else if (e.getSource() == bugReport) {
			Utility.submitBugReport("");
		} else if (e.getSource() == manual) {
			try {
				String directory = "";
				String theFile = "";
				theFile = "ATACS.html";
				Preferences biosimrc = Preferences.userRoot();
				String command = biosimrc.get("biosim.general.browser", "");
				if (System.getProperty("os.name").contentEquals("Linux")
						|| System.getProperty("os.name").toLowerCase().startsWith("mac os")) {
					directory = ENVVAR + "/docs/";
				} else {
					directory = ENVVAR + "\\docs\\";
				}
				File work = new File(directory);
				log.addText("Executing:\n" + command + " " + directory + theFile + "\n");
				Runtime exec = Runtime.getRuntime();
				exec.exec(command + " " + theFile, null, work);
			} catch (IOException e1) {
				JOptionPane.showMessageDialog(frame, "Unable to open manual.", "Error", JOptionPane.ERROR_MESSAGE);
			}
		}
		// if the exit menu item is selected
		else if (e.getSource() == exit) {
			exit();
		}
		// if the open popup menu is selected on a sim directory
		else if (e.getActionCommand().equals("openSim")) {
			try {
				openAnalysisView(tree.getFile(), true);
			} catch (Exception e0) {
			}
		} else if (e.getActionCommand().equals("openSynth")) {
			openSynth();
		} else if (e.getActionCommand().equals("openVerification")) {
			openVerify();
		} else if (e.getActionCommand().equals("convertToSBML")) {
			Translator t1 = new Translator();
			try {
				t1.convertLPN2SBML(tree.getFile(), "");
			} catch (BioSimException e1) {
				e1.printStackTrace();
			}
			t1.outputSBML();
			String theFile = "";
			if (tree.getFile().lastIndexOf('/') >= 0) {
				theFile = tree.getFile().substring(tree.getFile().lastIndexOf('/') + 1);
			}
			if (tree.getFile().lastIndexOf('\\') >= 0) {
				theFile = tree.getFile().substring(tree.getFile().lastIndexOf('\\') + 1);
			}
			// updateOpenSBML(theFile.replace(".lpn", ".xml"));
			addToTree(theFile.replace(".lpn", ".xml"));
		} else if (e.getActionCommand().equals("convertToVerilog")) {
			try {
				Lpn2verilog.convert(tree.getFile());
			} catch (BioSimException e1) {
				JOptionPane.showMessageDialog(atacsGui.frame, e1.getMessage(), e1.getTitle(), JOptionPane.ERROR_MESSAGE);
				e1.printStackTrace();
			}
			String theFile = "";
			if (tree.getFile().lastIndexOf('/') >= 0) {
				theFile = tree.getFile().substring(tree.getFile().lastIndexOf('/') + 1);
			}
			if (tree.getFile().lastIndexOf('\\') >= 0) {
				theFile = tree.getFile().substring(tree.getFile().lastIndexOf('\\') + 1);
			}
			addToTree(theFile.replace(".lpn", ".sv"));
		}

		else if (e.getActionCommand().equals("convertToLPN")) {
			// new BuildProperty();
			try {
				BuildProperty.buildProperty(tree.getFile());
			} catch (IOException e1) {
				e1.printStackTrace();
			} catch (RecognitionException e1) {
				e1.printStackTrace();
			} catch (BioSimException e1) {
				e1.printStackTrace();
			}
			String theFile = "";
			if (tree.getFile().lastIndexOf('/') >= 0) {
				theFile = tree.getFile().substring(tree.getFile().lastIndexOf('/') + 1);
			}
			if (tree.getFile().lastIndexOf('\\') >= 0) {
				theFile = tree.getFile().substring(tree.getFile().lastIndexOf('\\') + 1);
			}
			addToTree(theFile.replace(".prop", ".xml"));
		} else if (e.getActionCommand().equals("createAnalysis")) {
			try {
				createAnalysisView(tree.getFile(),true);
			} catch (Exception e1) {
				e1.printStackTrace();
				JOptionPane.showMessageDialog(frame, "You must select a valid model file for analysis.", "Error",
						JOptionPane.ERROR_MESSAGE);
			}
		}
		// if the synthesis popup menu is selected on a vhdl or lhpn file
		else if (e.getActionCommand().equals("createSynthesis")) {
			if (root != null) {
				for (int i = 0; i < tab.getTabCount(); i++) {
					if (getTitleAt(i).equals(GlobalConstants.getFilename(tree.getFile()))) {
						tab.setSelectedIndex(i);
						if (save(i, 0) == 0) {
							return;
						}
						break;
					}
				}
				String synthName = JOptionPane.showInputDialog(frame, "Enter Synthesis ID:", "Synthesis View ID",
						JOptionPane.PLAIN_MESSAGE);
				if (synthName != null && !synthName.trim().equals("")) {
					synthName = synthName.trim();
					try {
						if (overwrite(root + File.separator + synthName, synthName)) {
							new File(root + File.separator + synthName).mkdir();
							String sbmlFile = tree.getFile();
							String[] getFilename = GlobalConstants.splitPath(sbmlFile);
							String circuitFileNoPath = getFilename[getFilename.length - 1];
							try {
								FileOutputStream out = new FileOutputStream(
										new File(root + File.separator + synthName.trim()
										+ File.separator + synthName.trim() + ".syn"));
								out.write(("synthesis.file=" + circuitFileNoPath + "\n").getBytes());
								out.close();
							} catch (IOException e1) {
								JOptionPane.showMessageDialog(frame, "Unable to save parameter file!",
										"Error Saving File", JOptionPane.ERROR_MESSAGE);
							}
							try {
								FileInputStream in = new FileInputStream(
										new File(root + File.separator + circuitFileNoPath));
								FileOutputStream out = new FileOutputStream(
										new File(root + File.separator + synthName.trim()
										+ File.separator + circuitFileNoPath));
								int read = in.read();
								while (read != -1) {
									out.write(read);
									read = in.read();
								}
								in.close();
								out.close();
							} catch (Exception e1) {
								JOptionPane.showMessageDialog(frame, "Unable to copy circuit file!",
										"Error Saving File", JOptionPane.ERROR_MESSAGE);
							}
							addToTree(synthName.trim());
							String work = root + File.separator + synthName;
							String circuitFile = root + File.separator + synthName.trim()
							+ File.separator + circuitFileNoPath;
							JPanel synthPane = new JPanel();
							SynthesisViewATACS synth = new SynthesisViewATACS(work, circuitFile, log, this);
							synthPane.add(synth);
							addTab(synthName, synthPane, "Synthesis");
						}
					} catch (Exception e1) {
						JOptionPane.showMessageDialog(frame, "Unable to create Synthesis View directory.", "Error",
								JOptionPane.ERROR_MESSAGE);
					}
				}
			} else {
				JOptionPane.showMessageDialog(frame, "You must open or create a project first.", "Error",
						JOptionPane.ERROR_MESSAGE);
			}
		}
		// if the verify popup menu is selected on a vhdl or lhpn file
		else if (e.getActionCommand().equals("createVerify")) {
			if (root != null) {
				for (int i = 0; i < tab.getTabCount(); i++) {
					if (getTitleAt(i).equals(GlobalConstants.getFilename(tree.getFile()))) {
						tab.setSelectedIndex(i);
						if (save(i, 0) == 0) {
							return;
						}
						break;
					}
				}
				String verName = JOptionPane.showInputDialog(frame, "Enter Verification ID:", "Verification View ID",
						JOptionPane.PLAIN_MESSAGE);
				if (verName != null && !verName.trim().equals("")) {
					verName = verName.trim();
					// try {
					if (overwrite(root + File.separator + verName, verName)) {
						new File(root + File.separator + verName).mkdir();
						String sbmlFile = tree.getFile();
						String[] getFilename = GlobalConstants.splitPath(sbmlFile);
						String circuitFileNoPath = getFilename[getFilename.length - 1];
						try {
							FileOutputStream out = new FileOutputStream(new File(root + File.separator
									+ verName.trim() + File.separator + verName.trim() + ".ver"));
							out.write(("verification.file=" + circuitFileNoPath + "\n").getBytes());
							out.close();
						} catch (IOException e1) {
							JOptionPane.showMessageDialog(frame, "Unable to save parameter file!", "Error Saving File",
									JOptionPane.ERROR_MESSAGE);
						}
						addToTree(verName.trim());
						VerificationView verify = new VerificationView(root + File.separator + verName, verName,
								circuitFileNoPath, log, this, false, true);
						verify.save();
						addTab(verName, verify, "Verification");
					}
				}
			} else {
				JOptionPane.showMessageDialog(frame, "You must open or create a project first.", "Error",
						JOptionPane.ERROR_MESSAGE);
			}
		}
		// if the delete popup menu is selected
		else if (e.getActionCommand().contains("delete") || e.getSource() == delete) {
			for (String file : tree.getFiles()) {
				delete(file);
			}
		} else if (e.getActionCommand().equals("openLPN")) {
			openLPN();
		} 
		else if (e.getActionCommand().equals("modelEditor")) {
			// if the edit popup menu is selected on a dot file
			openModelEditor(false, true);
		}
		// if the edit popup menu is selected on a dot file
		else if (e.getActionCommand().equals("modelTextEditor")) {
			openModelEditor(true, true);
		}
		// if the edit popup menu is selected on an sbml file
		else if (e.getActionCommand().equals("sbmlEditor")) {
			openSBML(tree.getFile(), true);
		} else if (e.getActionCommand().equals("stateGraph")) {
			try {
				String directory = root + File.separator + getTitleAt(tab.getSelectedIndex());
				File work = new File(directory);
				for (String f : new File(directory).list()) {
					if (f.contains("_sg.dot")) {
						Runtime exec = Runtime.getRuntime();
						if (System.getProperty("os.name").contentEquals("Linux")) {
							log.addText("Executing:\ndotty " + directory + File.separator + f + "\n");
							exec.exec("dotty " + f, null, work);
						} else if (System.getProperty("os.name").toLowerCase().startsWith("mac os")) {
							log.addText("Executing:\nopen " + directory + File.separator + f + "\n");
							exec.exec("open " + f, null, work);
						} else {
							log.addText("Executing:\ndotty " + directory + File.separator + f + "\n");
							exec.exec("dotty " + f, null, work);
						}
						return;
					}
				}
				JOptionPane.showMessageDialog(frame, "State graph file has not been generated.", "Error",
						JOptionPane.ERROR_MESSAGE);
			} catch (Exception e1) {
				JOptionPane.showMessageDialog(frame, "Error viewing state graph.", "Error", JOptionPane.ERROR_MESSAGE);
			}
		} else if (e.getActionCommand().equals("graphTree")) {
			String directory = "";
			String theFile = "";
			String filename = tree.getFile();
			if (filename.lastIndexOf('/') >= 0) {
				directory = filename.substring(0, filename.lastIndexOf('/') + 1);
				theFile = filename.substring(filename.lastIndexOf('/') + 1);
			}
			if (filename.lastIndexOf('\\') >= 0) {
				directory = filename.substring(0, filename.lastIndexOf('\\') + 1);
				theFile = filename.substring(filename.lastIndexOf('\\') + 1);
			}
			for (int i = 0; i < tab.getTabCount(); i++) {
				if (getTitleAt(i).equals(GlobalConstants.getFilename(tree.getFile()))) {
					tab.setSelectedIndex(i);
					if (save(i, 0) == 0) {
						return;
					}
					break;
				}
			}
			File work = new File(directory);
			String out = theFile;
			try {
				if (out.contains(".lpn")) {
					String file = theFile;
					String[] findTheFile = file.split("\\.");
					theFile = findTheFile[0] + ".dot";
					File dot = new File(root + File.separator + theFile);
					dot.delete();
					LPN lhpn = new LPN();
					lhpn.addObserver(this);
					try {
						lhpn.load(directory + File.separator + theFile);
					} catch (BioSimException e1) {
						JOptionPane.showMessageDialog(atacsGui.frame, e1.getMessage(), e1.getTitle(),
								JOptionPane.ERROR_MESSAGE);
						e1.printStackTrace();
					}
					lhpn.printDot(directory + File.separator + file);
					// String cmd = "atacs -cPllodpl " + file;
					Runtime exec = Runtime.getRuntime();
					// Process ATACS = exec.exec(cmd, null, work);
					// ATACS.waitFor();
					// log.addText("Executing:\n" + cmd);
					if (dot.exists()) {
						Preferences biosimrc = Preferences.userRoot();
						String command = biosimrc.get("biosim.general.graphviz", "");
						log.addText(command + " " + root + File.separator + theFile + "\n");
						exec.exec(command + theFile, null, work);
					} else {
						File log = new File(root + File.separator + "atacs.log");
						BufferedReader input = new BufferedReader(new FileReader(log));
						String line = null;
						JTextArea messageArea = new JTextArea();
						while ((line = input.readLine()) != null) {
							messageArea.append(line);
							messageArea.append(System.getProperty("line.separator"));
						}
						input.close();
						messageArea.setLineWrap(true);
						messageArea.setWrapStyleWord(true);
						messageArea.setEditable(false);
						JScrollPane scrolls = new JScrollPane();
						scrolls.setMinimumSize(new Dimension(500, 500));
						scrolls.setPreferredSize(new Dimension(500, 500));
						scrolls.setViewportView(messageArea);
						JOptionPane.showMessageDialog(frame, scrolls, "Log", JOptionPane.INFORMATION_MESSAGE);
					}
					return;
				}
				if (out.length() > 4 && out.substring(out.length() - 5, out.length()).equals(".sbml")) {
					out = out.substring(0, out.length() - 5);
				} else if (out.length() > 3 && out.substring(out.length() - 4, out.length()).equals(".xml")) {
					out = out.substring(0, out.length() - 4);
				} else if (out.length() > 3 && out.substring(out.length() - 4, out.length()).equals(".gcm")) {
					try {
						if (System.getProperty("os.name").contentEquals("Linux")) {
							log.addText("Executing:\ndotty " + directory + theFile + "\n");
							Runtime exec = Runtime.getRuntime();
							exec.exec("dotty " + theFile, null, work);
						} else if (System.getProperty("os.name").toLowerCase().startsWith("mac os")) {
							log.addText("Executing:\nopen " + directory + theFile + "\n");
							Runtime exec = Runtime.getRuntime();
							exec.exec("cp " + theFile + " " + theFile + ".dot", null, work);
							exec = Runtime.getRuntime();
							exec.exec("open " + theFile + ".dot", null, work);
						} else {
							log.addText("Executing:\ndotty " + directory + theFile + "\n");
							Runtime exec = Runtime.getRuntime();
							exec.exec("dotty " + theFile, null, work);
						}
						return;
					} catch (Exception e1) {
						JOptionPane.showMessageDialog(frame, "Unable to view this gcm file.", "Error",
								JOptionPane.ERROR_MESSAGE);
						return;
					}
				}
				// new Run(null);
				JCheckBox dummy = new JCheckBox();
				dummy.setSelected(false);
//				JList empty = new JList();
				// JRadioButton emptyButton = new JRadioButton();
//				Run.createProperties(0, 0, 0, "Print Interval", 1, 1, 1, 1, 0, directory, 314159, 1, 1, new String[0],
//						"tsd.printer", "amount", "false", GlobalConstants.splitPath(directory + theFile),
//						"none", frame, directory + theFile, 0.1, 0.1, 0.1, 15, 2.0, empty, empty, empty, null, false,
//						false, false);
				log.addText("Executing:\n" + Executables.reb2sacExecutable + " --target.encoding=dot --out=" + directory + out
						+ ".dot " + directory + theFile + "\n");
				Runtime exec = Runtime.getRuntime();
				Process graph = exec.exec(Executables.reb2sacExecutable + " --target.encoding=dot --out=" + out + ".dot " + theFile,
						envp, work);
				String error = "";
				String output = "";
				InputStream reb = graph.getErrorStream();
				int read = reb.read();
				while (read != -1) {
					error += (char) read;
					read = reb.read();
				}
				reb.close();
				reb = graph.getInputStream();
				read = reb.read();
				while (read != -1) {
					output += (char) read;
					read = reb.read();
				}
				reb.close();
				if (!output.equals("")) {
					log.addText("Output:\n" + output + "\n");
				}
				if (!error.equals("")) {
					log.addText("Errors:\n" + error + "\n");
				}
				graph.waitFor();
				if (error.equals("")) {
					if (System.getProperty("os.name").contentEquals("Linux")) {
						log.addText("Executing:\ndotty " + directory + out + ".dot\n");
						exec.exec("dotty " + out + ".dot", null, work);
					} else if (System.getProperty("os.name").toLowerCase().startsWith("mac os")) {
						log.addText("Executing:\nopen " + directory + out + ".dot\n");
						exec.exec("open " + out + ".dot", null, work);
					} else {
						log.addText("Executing:\ndotty " + directory + out + ".dot\n");
						exec.exec("dotty " + out + ".dot", null, work);
					}
				}
				String remove;
				if (theFile.substring(theFile.length() - 4).equals("sbml")) {
					remove = (directory + theFile).substring(0, (directory + theFile).length() - 4) + "properties";
				} else {
					remove = (directory + theFile).substring(0, (directory + theFile).length() - 4) + ".properties";
				}
				System.gc();
				new File(remove).delete();
			} catch (InterruptedException e1) {
				JOptionPane.showMessageDialog(frame, "Error graphing SBML file.", "Error", JOptionPane.ERROR_MESSAGE);
			} catch (IOException e1) {
				JOptionPane.showMessageDialog(frame, "Error graphing SBML file.", "Error", JOptionPane.ERROR_MESSAGE);
			}
		} 
		// if the graph popup menu is selected on an sbml file
		else if (e.getActionCommand().equals("graph")) {
			String directory = "";
			String theFile = "";
			String filename = tree.getFile();
			if (filename.lastIndexOf('/') >= 0) {
				directory = filename.substring(0, filename.lastIndexOf('/') + 1);
				theFile = filename.substring(filename.lastIndexOf('/') + 1);
			}
			if (filename.lastIndexOf('\\') >= 0) {
				directory = filename.substring(0, filename.lastIndexOf('\\') + 1);
				theFile = filename.substring(filename.lastIndexOf('\\') + 1);
			}
			for (int i = 0; i < tab.getTabCount(); i++) {
				if (getTitleAt(i).equals(GlobalConstants.getFilename(tree.getFile()))) {
					tab.setSelectedIndex(i);
					if (save(i, 0) == 0) {
						return;
					}
					break;
				}
			}
			// }
			File work = new File(directory);
			String out = theFile;
			try {
				if (out.contains(".lpn")) {
					String file = theFile;
					String[] findTheFile = file.split("\\.");
					theFile = findTheFile[0] + ".dot";
					File dot = new File(root + File.separator + theFile);
					dot.delete();
					LPN lhpn = new LPN();
					lhpn.addObserver(this);
					try {
						lhpn.load(root + File.separator + file);
					} catch (BioSimException e1) {
						JOptionPane.showMessageDialog(atacsGui.frame, e1.getMessage(), e1.getTitle(),
								JOptionPane.ERROR_MESSAGE);
						e1.printStackTrace();
					}
					lhpn.printDot(root + File.separator + theFile);
					// String cmd = "atacs -cPllodpl " + file;
					Runtime exec = Runtime.getRuntime();
					// Process ATACS = exec.exec(cmd, null, work);
					// ATACS.waitFor();
					// log.addText("Executing:\n" + cmd);
					if (dot.exists()) {
						Preferences biosimrc = Preferences.userRoot();
						String command = biosimrc.get("biosim.general.graphviz", "");
						log.addText(command + " " + root + File.separator + theFile + "\n");
						exec.exec(command + " " + theFile, null, work);
					} else {
						File log = new File(root + File.separator + "atacs.log");
						BufferedReader input = new BufferedReader(new FileReader(log));
						String line = null;
						JTextArea messageArea = new JTextArea();
						while ((line = input.readLine()) != null) {
							messageArea.append(line);
							messageArea.append(System.getProperty("line.separator"));
						}
						input.close();
						messageArea.setLineWrap(true);
						messageArea.setWrapStyleWord(true);
						messageArea.setEditable(false);
						JScrollPane scrolls = new JScrollPane();
						scrolls.setMinimumSize(new Dimension(500, 500));
						scrolls.setPreferredSize(new Dimension(500, 500));
						scrolls.setViewportView(messageArea);
						JOptionPane.showMessageDialog(frame, scrolls, "Log", JOptionPane.INFORMATION_MESSAGE);
					}
					return;
				}
				if (out.length() > 4 && out.substring(out.length() - 5, out.length()).equals(".sbml")) {
					out = out.substring(0, out.length() - 5);
				} else if (out.length() > 3 && out.substring(out.length() - 4, out.length()).equals(".xml")) {
					out = out.substring(0, out.length() - 4);
				} else if (out.length() > 3 && out.substring(out.length() - 4, out.length()).equals(".gcm")) {
					try {
						if (System.getProperty("os.name").contentEquals("Linux")) {
							log.addText("Executing:\ndotty " + directory + theFile + "\n");
							Runtime exec = Runtime.getRuntime();
							exec.exec("dotty " + theFile, null, work);
						} else if (System.getProperty("os.name").toLowerCase().startsWith("mac os")) {
							log.addText("Executing:\nopen " + directory + theFile + "\n");
							Runtime exec = Runtime.getRuntime();
							exec.exec("cp " + theFile + " " + theFile + ".dot", null, work);
							exec = Runtime.getRuntime();
							exec.exec("open " + theFile + ".dot", null, work);
						} else {
							log.addText("Executing:\ndotty " + directory + theFile + "\n");
							Runtime exec = Runtime.getRuntime();
							exec.exec("dotty " + theFile, null, work);
						}
						return;
					} catch (Exception e1) {
						JOptionPane.showMessageDialog(frame, "Unable to view this gcm file.", "Error",
								JOptionPane.ERROR_MESSAGE);
						return;
					}
				}
				// new Run(null);
				JCheckBox dummy = new JCheckBox();
				dummy.setSelected(false);
//				JList empty = new JList();
				// JRadioButton emptyButton = new JRadioButton();
//				Run.createProperties(0, 0, 0, "Print Interval", 1, 1, 1, 1, 0, directory, 314159, 1, 1, new String[0],
//						"tsd.printer", "amount", "false", GlobalConstants.splitPath(directory + theFile),
//						"none", frame, directory + theFile, 0.1, 0.1, 0.1, 15, 2.0, empty, empty, empty, null, false,
//						false, false);
				log.addText("Executing:\n" + Executables.reb2sacExecutable + " --target.encoding=dot --out=" + directory + out
						+ ".dot " + directory + theFile + "\n");
				Runtime exec = Runtime.getRuntime();
				Process graph = exec.exec(Executables.reb2sacExecutable + " --target.encoding=dot --out=" + out + ".dot " + theFile,
						null, work);
				String error = "";
				String output = "";
				InputStream reb = graph.getErrorStream();
				int read = reb.read();
				while (read != -1) {
					error += (char) read;
					read = reb.read();
				}
				reb.close();
				reb = graph.getInputStream();
				read = reb.read();
				while (read != -1) {
					output += (char) read;
					read = reb.read();
				}
				reb.close();
				if (!output.equals("")) {
					log.addText("Output:\n" + output + "\n");
				}
				if (!error.equals("")) {
					log.addText("Errors:\n" + error + "\n");
				}
				graph.waitFor();
				if (error.equals("")) {
					if (System.getProperty("os.name").contentEquals("Linux")) {
						log.addText("Executing:\ndotty " + directory + out + ".dot\n");
						exec.exec("dotty " + out + ".dot", null, work);
					} else if (System.getProperty("os.name").toLowerCase().startsWith("mac os")) {
						log.addText("Executing:\nopen " + directory + out + ".dot\n");
						exec.exec("open " + out + ".dot", null, work);
					} else {
						log.addText("Executing:\ndotty " + directory + out + ".dot\n");
						exec.exec("dotty " + out + ".dot", null, work);
					}
				}
				String remove;
				if (theFile.substring(theFile.length() - 4).equals("sbml")) {
					remove = (directory + theFile).substring(0, (directory + theFile).length() - 4) + "properties";
				} else {
					remove = (directory + theFile).substring(0, (directory + theFile).length() - 4) + ".properties";
				}
				System.gc();
				new File(remove).delete();
			} catch (IOException e1) {
				JOptionPane.showMessageDialog(frame, "Error graphing sbml file.", "Error", JOptionPane.ERROR_MESSAGE);
			} catch (InterruptedException e1) {
				JOptionPane.showMessageDialog(frame, "Error graphing sbml file.", "Error", JOptionPane.ERROR_MESSAGE);
			}
		}
		// if the browse popup menu is selected on an sbml file
		else if (e.getActionCommand().equals("browse")) {
			String directory;
			String theFile;
			String filename = tree.getFile();
			directory = "";
			theFile = "";
			if (filename.lastIndexOf('/') >= 0) {
				directory = filename.substring(0, filename.lastIndexOf('/') + 1);
				theFile = filename.substring(filename.lastIndexOf('/') + 1);
			}
			if (filename.lastIndexOf('\\') >= 0) {
				directory = filename.substring(0, filename.lastIndexOf('\\') + 1);
				theFile = filename.substring(filename.lastIndexOf('\\') + 1);
			}
			for (int i = 0; i < tab.getTabCount(); i++) {
				if (getTitleAt(i).equals(GlobalConstants.getFilename(tree.getFile()))) {
					tab.setSelectedIndex(i);
					if (save(i, 0) == 0) {
						return;
					}
					break;
				}
			}
			// }
			File work = new File(directory);
			String out = theFile;
			if (out.length() > 4 && out.substring(out.length() - 5, out.length()).equals(".sbml")) {
				out = out.substring(0, out.length() - 5);
			} else if (out.length() > 3 && out.substring(out.length() - 4, out.length()).equals(".xml")) {
				out = out.substring(0, out.length() - 4);
			}
			try {
				// new Run(null);
				JCheckBox dummy = new JCheckBox();
//				JList empty = new JList();
				dummy.setSelected(false);
//				Run.createProperties(0, 0, 0.0, "Print Interval", 1.0, 1.0, 1.0, 1.0, 0, directory, 314159L, 1, 1,
//						new String[0], "tsd.printer", "amount", "false",
//						GlobalConstants.splitPath(directory + theFile), "none", frame, directory + theFile, 0.1,
//						0.1, 0.1, 15, 2.0, empty, empty, empty, null, false, false, false);
				log.addText("Executing:\n" + Executables.reb2sacExecutable + " --target.encoding=xhtml --out=" + directory + out
						+ ".xhtml " + directory + theFile + "\n");
				Runtime exec = Runtime.getRuntime();
				Process browse = exec.exec(
						Executables.reb2sacExecutable + " --target.encoding=xhtml --out=" + out + ".xhtml " + theFile, envp, work);
				String error = "";
				String output = "";
				InputStream reb = browse.getErrorStream();
				int read = reb.read();
				while (read != -1) {
					error += (char) read;
					read = reb.read();
				}
				reb.close();
				reb = browse.getInputStream();
				read = reb.read();
				while (read != -1) {
					output += (char) read;
					read = reb.read();
				}
				reb.close();
				if (!output.equals("")) {
					log.addText("Output:\n" + output + "\n");
				}
				if (!error.equals("")) {
					log.addText("Errors:\n" + error + "\n");
				}
				browse.waitFor();

				Preferences biosimrc = Preferences.userRoot();
				String command = biosimrc.get("biosim.general.browser", "");
				if (error.equals("")) {
					log.addText("Executing:\n" + command + " " + directory + out + ".xhtml\n");
					exec.exec(command + " " + out + ".xhtml", null, work);
				}
				String remove;
				if (theFile.substring(theFile.length() - 4).equals("sbml")) {
					remove = (directory + theFile).substring(0, (directory + theFile).length() - 4) + "properties";
				} else {
					remove = (directory + theFile).substring(0, (directory + theFile).length() - 4) + ".properties";
				}
				System.gc();
				new File(remove).delete();
			} catch (Exception e1) {
				JOptionPane.showMessageDialog(frame, "Error viewing SBML file in a browser.", "Error",
						JOptionPane.ERROR_MESSAGE);
			}
		}
		// if the save button is pressed on the Tool Bar
		else if (e.getActionCommand().equals("save")) {
			Component comp = tab.getSelectedComponent();
			// int index = tab.getSelectedIndex();
			if (comp instanceof LHPNEditor) {
				((LHPNEditor) comp).save();
			} else if (comp instanceof ModelEditor) {
				((ModelEditor) comp).save(false);
			} else if (comp instanceof Graph) {
				((Graph) comp).save();
			} else if (comp instanceof VerificationView) {
				((VerificationView) comp).save();
			} else if (comp instanceof JTabbedPane) {
				if (comp instanceof SynthesisView) {
					((SynthesisView) comp).save();
				} else {
					for (Component component : ((JTabbedPane) comp).getComponents()) {
						int index = ((JTabbedPane) comp).getSelectedIndex();
						if (component instanceof Graph) {
							((Graph) component).save();
						} else if (component instanceof LearnView) {
							((LearnView) component).save();
						} else if (component instanceof DataManager) {
							((DataManager) component).saveChanges(((JTabbedPane) comp).getTitleAt(index));
						}
						/*
						 * else if (component instanceof SBML_Editor) {
						 * ((SBML_Editor) component).save(false, "", true,
						 * true); }
						 */
						else if (component instanceof ModelEditor) {
							((ModelEditor) component).saveParams(false, "", true, null);
						} else if (component instanceof AnalysisView) {
							((AnalysisView) component).save();
						} else if (component instanceof MovieContainer) {
							((MovieContainer) component).savePreferences();
						}
					}
				}
			}
			if (comp instanceof JPanel) {
				if (comp.getName().equals("Synthesis")) {
					// ((Synthesis) tab.getSelectedComponent()).save();
					Component[] array = ((JPanel) comp).getComponents();
					((SynthesisViewATACS) array[0]).save();
				}
			} else if (comp instanceof JScrollPane) {
				String fileName = getTitleAt(tab.getSelectedIndex());
				try {
					File output = new File(root + File.separator + fileName);
					output.createNewFile();
					FileOutputStream outStream = new FileOutputStream(output);
					Component[] array = ((JScrollPane) comp).getComponents();
					array = ((JViewport) array[0]).getComponents();
					if (array[0] instanceof JTextArea) {
						String text = ((JTextArea) array[0]).getText();
						char[] chars = text.toCharArray();
						for (int j = 0; j < chars.length; j++) {
							outStream.write(chars[j]);
						}
					}
					outStream.close();
					log.addText("Saving file:\n" + root + File.separator + fileName);
					this.updateAsyncViews(fileName);
				} catch (Exception e1) {
					JOptionPane.showMessageDialog(frame, "Error saving file " + fileName, "Error",
							JOptionPane.ERROR_MESSAGE);
				}
			}
		}
		// if the save as button is pressed on the Tool Bar
		else if (e.getActionCommand().equals("saveas")) {
			Component comp = tab.getSelectedComponent();
			// int index = tab.getSelectedIndex();
			if (comp instanceof LHPNEditor) {
				String newName = JOptionPane.showInputDialog(frame, "Enter LPN name:", "LPN Name",
						JOptionPane.PLAIN_MESSAGE);
				if (newName == null) {
					return;
				}
				if (!newName.endsWith(".lpn") && !newName.endsWith(".xml")) {
					newName = newName + ".lpn";
				}
				((LHPNEditor) comp).saveAs(newName);
				if (newName.endsWith(".lpn")) {
					tab.setTitleAt(tab.getSelectedIndex(), newName);
				}
			} else if (comp instanceof ModelEditor) {
				String newName = JOptionPane.showInputDialog(frame, "Enter model name:", "Model Name",
						JOptionPane.PLAIN_MESSAGE);
				if (newName == null) {
					return;
				}
				if (newName.contains(".gcm")) {
					newName = newName.replace(".gcm", "");
				}
				if (newName.contains(".xml")) {
					newName = newName.replace(".xml", "");
				}
				if (newName.endsWith(".lpn")) {
					((ModelEditor) comp).saveAsLPN(newName);
				} else if (newName.endsWith(".sv")) {
					((ModelEditor) comp).saveAsVerilog(newName);
				} else {
					((ModelEditor) comp).saveAs(newName);
				}
			} else if (comp instanceof Graph) {
				((Graph) comp).saveAs();
			} else if (comp instanceof VerificationView) {
				((VerificationView) comp).saveAs();
			} else if (comp instanceof JTabbedPane) {
				Component component = ((JTabbedPane) comp).getSelectedComponent();
				if (component instanceof Graph) {
					((Graph) component).saveAs();
				}
			} else if (comp instanceof JPanel) {
				if (comp.getName().equals("Synthesis")) {
					Component[] array = ((JPanel) comp).getComponents();
					((SynthesisViewATACS) array[0]).saveAs();
				}
			} else if (comp instanceof JScrollPane) {
				String fileName = getTitleAt(tab.getSelectedIndex());
				String newName = "";
				if (fileName.endsWith(".vhd")) {
					newName = JOptionPane.showInputDialog(frame, "Enter VHDL name:", "VHDL Name",
							JOptionPane.PLAIN_MESSAGE);
					if (newName == null) {
						return;
					}
					if (!newName.endsWith(".vhd")) {
						newName = newName + ".vhd";
					}
				}
				if (fileName.endsWith(".prop")) { // DK
					newName = JOptionPane.showInputDialog(frame, "Enter Property name:", "Property Name",
							JOptionPane.PLAIN_MESSAGE);
					if (newName == null) {
						return;
					}
					if (!newName.endsWith(".prop")) {
						newName = newName + ".prop";
					}
				} else if (fileName.endsWith(".s")) {
					newName = JOptionPane.showInputDialog(frame, "Enter Assembly File Name:", "Assembly File Name",
							JOptionPane.PLAIN_MESSAGE);
					if (newName == null) {
						return;
					}
					if (!newName.endsWith(".s")) {
						newName = newName + ".s";
					}
				} else if (fileName.endsWith(".inst")) {
					newName = JOptionPane.showInputDialog(frame, "Enter Instruction File Name:",
							"Instruction File Name", JOptionPane.PLAIN_MESSAGE);
					if (newName == null) {
						return;
					}
					if (!newName.endsWith(".inst")) {
						newName = newName + ".inst";
					}
				} else if (fileName.endsWith(".g")) {
					newName = JOptionPane.showInputDialog(frame, "Enter Petri net name:", "Petri net Name",
							JOptionPane.PLAIN_MESSAGE);
					if (newName == null) {
						return;
					}
					if (!newName.endsWith(".g")) {
						newName = newName + ".g";
					}
				} else if (fileName.endsWith(".csp")) {
					newName = JOptionPane.showInputDialog(frame, "Enter CSP name:", "CSP Name",
							JOptionPane.PLAIN_MESSAGE);
					if (newName == null) {
						return;
					}
					if (!newName.endsWith(".csp")) {
						newName = newName + ".csp";
					}
				} else if (fileName.endsWith(".hse")) {
					newName = JOptionPane.showInputDialog(frame, "Enter HSE name:", "HSE Name",
							JOptionPane.PLAIN_MESSAGE);
					if (newName == null) {
						return;
					}
					if (!newName.endsWith(".hse")) {
						newName = newName + ".hse";
					}
				} else if (fileName.endsWith(".unc")) {
					newName = JOptionPane.showInputDialog(frame, "Enter UNC name:", "UNC Name",
							JOptionPane.PLAIN_MESSAGE);
					if (newName == null) {
						return;
					}
					if (!newName.endsWith(".unc")) {
						newName = newName + ".unc";
					}
				} else if (fileName.endsWith(".rsg")) {
					newName = JOptionPane.showInputDialog(frame, "Enter RSG name:", "RSG Name",
							JOptionPane.PLAIN_MESSAGE);
					if (newName == null) {
						return;
					}
					if (!newName.endsWith(".rsg")) {
						newName = newName + ".rsg";
					}
				}
				try {
					File output = new File(root + File.separator + newName);
					output.createNewFile();
					FileOutputStream outStream = new FileOutputStream(output);
					Component[] array = ((JScrollPane) comp).getComponents();
					array = ((JViewport) array[0]).getComponents();
					if (array[0] instanceof JTextArea) {
						String text = ((JTextArea) array[0]).getText();
						char[] chars = text.toCharArray();
						for (int j = 0; j < chars.length; j++) {
							outStream.write(chars[j]);
						}
					}
					outStream.close();
					log.addText("Saving file:\n" + root + File.separator + newName);
					File oldFile = new File(root + File.separator + fileName);
					oldFile.delete();
					tab.setTitleAt(tab.getSelectedIndex(), newName);
					addToTree(newName);
				} catch (Exception e1) {
					JOptionPane.showMessageDialog(frame, "Error saving file " + newName, "Error",
							JOptionPane.ERROR_MESSAGE);
				}
			}
		}
		// if the run button is selected on the tool bar
		else if (e.getActionCommand().equals("run")) {
			Component comp = tab.getSelectedComponent();
			// int index = tab.getSelectedIndex();
			if (comp instanceof JTabbedPane) {
				// int index = -1;
				if (comp instanceof SynthesisView) {
					// TODO: why here?
					//synthesizeSBOL((SynthesisView) comp);
				} else {
					for (int i = 0; i < ((JTabbedPane) comp).getTabCount(); i++) {
						Component component = ((JTabbedPane) comp).getComponent(i);
						if (component instanceof AnalysisView) {
							((AnalysisView) component).executeRun();
							break;
						} else if (component instanceof LearnView) {
							((LearnView) component).save();
							new Thread((LearnView) component).start();
							break;
						}
					}
				}
			} else if (comp.getName().equals("Verification")) {
				if (comp instanceof VerificationView) {
					((VerificationView) comp).save();
					new Thread((VerificationView) comp).start();
				} else {
					// Not sure if this is necessary anymore
					Component[] array = ((JPanel) comp).getComponents();
					((VerificationView) array[0]).save();
					new Thread((VerificationView) array[0]).start();
				}
			} else if (comp.getName().equals("Synthesis")) {
				if (comp instanceof SynthesisViewATACS) {
					((SynthesisViewATACS) comp).save();
					new Thread((SynthesisViewATACS) comp).start();
				} else {
					// Not sure if this is necessary anymore
					Component[] array = ((JPanel) comp).getComponents();
					((SynthesisViewATACS) array[0]).save();
					new Thread((SynthesisViewATACS) array[0]).start();
				}
			}
		} else if (e.getActionCommand().equals("refresh")) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof JTabbedPane) {
				Component component = ((JTabbedPane) comp).getSelectedComponent();
				if (component instanceof Graph) {
					((Graph) component).refresh();
				}
			} else if (comp instanceof Graph) {
				((Graph) comp).refresh();
			}
		} else if (e.getActionCommand().equals("export")) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof Graph) {
				((Graph) comp).export();
			} else if (comp instanceof ModelEditor) {
				((ModelEditor) comp).exportSBML();
				// TODO: should give choice of SBML or SBOL
			} else if (comp instanceof JTabbedPane) {
				Component component = ((JTabbedPane) comp).getSelectedComponent();
				if (component instanceof Graph) {
					((Graph) component).export();
				}
				// TODO: what about export of movie?
			}
		}
		// if the new menu item is selected
		else if (e.getSource() == newProj) {
			createProject(e);
		}
		// if the open project menu item is selected
		else if (e.getSource() == pref) {
			PreferencesDialog.showPreferences(frame);
			EditPreferences editPreferences = new EditPreferences(frame, true);
			editPreferences.preferences();
			tree.setExpandibleIcons(!IBioSimPreferences.INSTANCE.isPlusMinusIconsEnabled());
		} else if (e.getSource() == clearRecent) {
			removeAllRecentProjects();
		} else if ((e.getSource() == openProj) || (e.getSource() == recentProjects[0])
				|| (e.getSource() == recentProjects[1]) || (e.getSource() == recentProjects[2])
				|| (e.getSource() == recentProjects[3]) || (e.getSource() == recentProjects[4])
				|| (e.getSource() == recentProjects[5]) || (e.getSource() == recentProjects[6])
				|| (e.getSource() == recentProjects[7]) || (e.getSource() == recentProjects[8])
				|| (e.getSource() == recentProjects[9])) {
			openProject(e);
		}
		// if the new circuit model menu item is selected
		else if (e.getSource() == newSBMLModel) {
			createModel(false, true);
		} 
		// if the new vhdl menu item is selected
		else if (e.getSource() == newVhdl) {
			newModel("VHDL", ".vhd");
		}
		// if the new petri net menu item is selected
		else if (e.getSource() == newG) {
			newModel("Petri Net", ".g");
		}
		// if the new lhpn menu item is selected
		else if (e.getSource() == newLhpn) {
			createLPN();
		} 
		// if the new csp menu item is selected
		else if (e.getSource() == newCsp) {
			newModel("CSP", ".csp");
		}
		// if the new hse menu item is selected
		else if (e.getSource() == newHse) {
			newModel("Handshaking Expansion", ".hse");
		}
		// if the new unc menu item is selected
		else if (e.getSource() == newUnc) {
			newModel("Extended Burst Mode Machine", ".unc");
		}
		// if the new rsg menu item is selected
		else if (e.getSource() == newRsg) {
			newModel("Reduced State Graph", ".rsg");
		}
		else if (e.getSource().equals(importSedml)) {
			importSEDML();
		}
		// if the import sbml menu item is selected
		else if (e.getSource() == importSbml) {
			importSBML(null);
		} 
		// if the import dot menu item is selected
		/*
		 * else if (e.getSource() == importDot) { importGCM(); }
		 */
		// if the import vhdl menu item is selected
		else if (e.getSource() == importVhdl) {
			importFile("VHDL", ".vhd", ".vhd");
		} else if (e.getSource() == importLpn) {
			importLPN();
		} else if (e.getSource() == importG) {
			importFile("Petri Net", ".g", ".g");
		}
		// if the import csp menu item is selected
		else if (e.getSource() == importCsp) {
			importFile("CSP", ".csp", ".csp");
		}
		// if the import hse menu item is selected
		else if (e.getSource() == importHse) {
			importFile("Handshaking Expansion", ".hse", ".hse");
		}
		// if the import unc menu item is selected
		else if (e.getSource() == importUnc) {
			importFile("Extended Burst State Machine", ".unc", ".unc");
		}
		// if the import rsg menu item is selected
		else if (e.getSource() == importRsg) {
			importFile("Reduced State Graph", ".rsg", ".rsg");
		}
		// if the Graph data menu item is clicked
		else if (e.getSource() == graph) {
			createGraph();
		} else if (e.getSource() == probGraph) {
			createHistogram();
		} else if (e.getActionCommand().equals("viewModel")) {
			viewModel();
		} else if (e.getSource() == select) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof ModelEditor) {
				((ModelEditor) comp).select();
			}
		} else if (e.getSource() == cut) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof ModelEditor) {
				((ModelEditor) comp).cut();
			}
		} else if (e.getSource() == addModule) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof ModelEditor) {
				((ModelEditor) comp).addComponent();
			}
		} else if (e.getSource() == addVariable) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof ModelEditor) {
				((ModelEditor) comp).addVariable();
			}
		} else if (e.getSource() == addBoolean) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof ModelEditor) {
				((ModelEditor) comp).addBoolean();
			}
		} else if (e.getSource() == addPlace) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof ModelEditor) {
				((ModelEditor) comp).addPlace();
			}
		} else if (e.getSource() == addTransition) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof ModelEditor) {
				((ModelEditor) comp).addTransition();
			}
		} else if (e.getSource() == addRule) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof ModelEditor) {
				((ModelEditor) comp).addRule();
			}
		} else if (e.getSource() == addConstraint) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof ModelEditor) {
				((ModelEditor) comp).addConstraint();
			}
		} else if (e.getSource() == moveLeft) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof ModelEditor) {
				((ModelEditor) comp).moveLeft();
			}
		} else if (e.getSource() == moveRight) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof ModelEditor) {
				((ModelEditor) comp).moveRight();
			}
		} else if (e.getSource() == moveUp) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof ModelEditor) {
				((ModelEditor) comp).moveUp();
			}
		} else if (e.getSource() == moveDown) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof ModelEditor) {
				((ModelEditor) comp).moveDown();
			}
		} else if (e.getSource() == undo) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof ModelEditor) {
				((ModelEditor) comp).undo();
			}
		} else if (e.getSource() == redo) {
			Component comp = tab.getSelectedComponent();
			if (comp instanceof ModelEditor) {
				((ModelEditor) comp).redo();
			}
		} else if (e.getActionCommand().equals("copy") || e.getSource() == copy) {
			copy();
		} else if (e.getActionCommand().equals("rename") || e.getSource() == rename) {
			rename();
		} else if (e.getActionCommand().equals("openGraph")) {
			openGraph();
		} else if (e.getActionCommand().equals("openHistogram")) {
			openHistogram();
		}
		enableTabMenu(tab.getSelectedIndex());
		enableTreeMenu();
	}

	private void createLPN() {
		try {
			String lhpnName = JOptionPane.showInputDialog(frame, "Enter LPN Model ID:", "Model ID",
					JOptionPane.PLAIN_MESSAGE);
			if (lhpnName != null && !lhpnName.trim().equals("")) {
				lhpnName = lhpnName.trim();
				if (lhpnName.length() > 3) {
					if (!lhpnName.substring(lhpnName.length() - 4).equals(".lpn")) {
						lhpnName += ".lpn";
					}
				} else {
					lhpnName += ".lpn";
				}
				String modelID = "";
				if (lhpnName.length() > 3) {
					if (lhpnName.substring(lhpnName.length() - 4).equals(".lpn")) {
						modelID = lhpnName.substring(0, lhpnName.length() - 4);
					} else {
						modelID = lhpnName.substring(0, lhpnName.length() - 3);
					}
				}
				if (!(IDpat.matcher(modelID).matches())) {
					JOptionPane.showMessageDialog(frame,
							"A model ID can only contain letters, digits, and underscores.\nIt also cannot start with a digit.",
							"Invalid ID", JOptionPane.ERROR_MESSAGE);
				} else {
					if (overwrite(root + File.separator + lhpnName, lhpnName)) {
						File f = new File(root + File.separator + lhpnName);
						f.createNewFile();
						LPN lpn = new LPN();
						lpn.addObserver(this);
						lpn.save(f.getAbsolutePath());
						int i = getTab(f.getName());
						if (i != -1) {
							tab.remove(i);
						}
						LHPNEditor lhpn = new LHPNEditor(root + File.separator, f.getName(), null, this);
						// lhpn.addMouseListener(this);
						addTab(f.getName(), lhpn, "LHPN Editor");
						addToTree(f.getName());
					}
				}
			}
		} catch (IOException e1) {
			JOptionPane.showMessageDialog(frame, "Unable to create new model.", "Error", JOptionPane.ERROR_MESSAGE);
		}
	}

	private void importLPN() {
		File importFile;
		Preferences biosimrc = Preferences.userRoot();
		if (biosimrc.get("biosim.general.import_dir", "").equals("")) {
			importFile = null;
		} else {
			importFile = new File(biosimrc.get("biosim.general.import_dir", ""));
		}
		String filename = Utility.browse(frame, importFile, null, JFileChooser.FILES_ONLY, "Import LPN", -1);
		if ((filename.length() > 1 && !filename.substring(filename.length() - 2, filename.length()).equals(".g"))
				&& (filename.length() > 3
						&& !filename.substring(filename.length() - 4, filename.length()).equals(".lpn"))) {
			JOptionPane.showMessageDialog(frame, "You must select a valid LPN file to import.", "Error",
					JOptionPane.ERROR_MESSAGE);
			return;
		} else if (!filename.equals("")) {
			biosimrc.put("biosim.general.import_dir", filename);
			String[] file = GlobalConstants.splitPath(filename);
			try {
				if (new File(filename).exists()) {
					file[file.length - 1] = file[file.length - 1].replaceAll("[^a-zA-Z0-9_.]+", "_");
					if (Character.isDigit(file[file.length - 1].charAt(0))) {
						file[file.length - 1] = "M" + file[file.length - 1];
					}
					if (checkFiles(root + File.separator + file[file.length - 1], filename.trim())) {
						if (overwrite(root + File.separator + file[file.length - 1],
								file[file.length - 1])) {
							// Identify which LPN format is imported.
							BufferedReader input = new BufferedReader(new FileReader(filename));
							String str;
							boolean lpnUSF = false;
							while ((str = input.readLine()) != null) {
								if (str.startsWith(".")) {
									break;
								} else if (str.startsWith("<")) {
									lpnUSF = true;
									break;
								} /*
								 * else {
								 * JOptionPane.showMessageDialog(frame,
								 * "LPN file format is not valid.", "Error",
								 * JOptionPane.ERROR_MESSAGE); break; }
								 */

							}
							input.close();
							if (!lpnUSF) {
								String outFileName = file[file.length - 1];

								FileOutputStream out = new FileOutputStream(
										new File(root + File.separator + outFileName));
								FileInputStream in = new FileInputStream(new File(filename));
								// log.addText(filename);
								int read = in.read();
								while (read != -1) {
									out.write(read);
									read = in.read();
								}
								in.close();
								out.close();
								addToTree(outFileName);
							} else {
								ANTLRFileStream in = new ANTLRFileStream(filename);
								PlatuGrammarLexer lexer = new PlatuGrammarLexer(in);
								TokenStream tokenStream = new CommonTokenStream(lexer);
								PlatuGrammarParser antlrParser = new PlatuGrammarParser(tokenStream);
								Set<LPN> lpnSet = antlrParser.lpn();
								for (LPN lpn : lpnSet) {
									lpn.save(root + File.separator + lpn.getLabel() + ".lpn");
									addToTree(lpn.getLabel() + ".lpn");
								}
							}
						}
					}
				}
				if (filename.substring(filename.length() - 2, filename.length()).equals(".g")) {
					// log.addText(filename + file[file.length - 1]);
					File work = new File(root);
					String oldName = root + File.separator + file[file.length - 1];
					// String newName = oldName.replace(".lpn",
					// "_NEW.g");
					Process atacs = Runtime.getRuntime().exec("atacs -lgsl " + oldName, null, work);
					atacs.waitFor();
					String lpnName = oldName.replace(".g", ".lpn");
					String newName = oldName.replace(".g", "_NEW.lpn");
					atacs = Runtime.getRuntime().exec("atacs -llsl " + lpnName, null, work);
					atacs.waitFor();
					lpnName = lpnName.replaceAll("[^a-zA-Z0-9_.]+", "_");
					if (Character.isDigit(lpnName.charAt(0))) {
						lpnName = "M" + lpnName;
					}
					FileOutputStream out = new FileOutputStream(new File(lpnName));
					FileInputStream in = new FileInputStream(new File(newName));
					int read = in.read();
					while (read != -1) {
						out.write(read);
						read = in.read();
					}
					in.close();
					out.close();
					new File(newName).delete();
					addToTree(file[file.length - 1].replace(".g", ".lpn"));
				}
			} catch (Exception e1) {
				e1.printStackTrace();
				JOptionPane.showMessageDialog(frame, "Unable to import file.", "Error", JOptionPane.ERROR_MESSAGE);
			}
		}
	}

	private void viewModel() {
		try {
			if (tree.getFile().length() >= 4 && tree.getFile().substring(tree.getFile().length() - 4).equals(".lpn")) {
				String filename = GlobalConstants.getFilename(tree.getFile());
				String[] findTheFile = filename.split("\\.");
				String theFile = findTheFile[0] + ".dot";
				File dot = new File(root + File.separator + theFile);
				dot.delete();
				LPN lhpn = new LPN();
				lhpn.addObserver(this);
				try {
					lhpn.load(tree.getFile());
				} catch (BioSimException e) {
					JOptionPane.showMessageDialog(atacsGui.frame, e.getMessage(), e.getTitle(), JOptionPane.ERROR_MESSAGE);
					e.printStackTrace();
				}
				lhpn.printDot(root + File.separator + theFile);
				File work = new File(root);
				Runtime exec = Runtime.getRuntime();
				if (dot.exists()) {
					Preferences biosimrc = Preferences.userRoot();
					String command = biosimrc.get("biosim.general.graphviz", "");
					log.addText(command + " " + root + File.separator + theFile + "\n");
					exec.exec(command + " " + theFile, null, work);
				} else {
					File log = new File(root + File.separator + "atacs.log");
					BufferedReader input = new BufferedReader(new FileReader(log));
					String line = null;
					JTextArea messageArea = new JTextArea();
					while ((line = input.readLine()) != null) {
						messageArea.append(line);
						messageArea.append(System.getProperty("line.separator"));
					}
					input.close();
					messageArea.setLineWrap(true);
					messageArea.setWrapStyleWord(true);
					messageArea.setEditable(false);
					JScrollPane scrolls = new JScrollPane();
					scrolls.setMinimumSize(new Dimension(500, 500));
					scrolls.setPreferredSize(new Dimension(500, 500));
					scrolls.setViewportView(messageArea);
					JOptionPane.showMessageDialog(frame, scrolls, "Log", JOptionPane.INFORMATION_MESSAGE);
				}
			} else if (tree.getFile().length() >= 2
					&& tree.getFile().substring(tree.getFile().length() - 2).equals(".g")) {
				String filename = GlobalConstants.getFilename(tree.getFile());
				String[] findTheFile = filename.split("\\.");
				String theFile = findTheFile[0] + ".dot";
				File dot = new File(root + File.separator + theFile);
				dot.delete();
				String cmd = "atacs -cPlgodpe " + filename;
				File work = new File(root);
				Runtime exec = Runtime.getRuntime();
				Process ATACS = exec.exec(cmd, null, work);
				ATACS.waitFor();
				log.addText("Executing:\n" + cmd);
				if (dot.exists()) {
					Preferences biosimrc = Preferences.userRoot();
					String command = biosimrc.get("biosim.general.graphviz", "");
					log.addText(command + " " + root + File.separator + theFile + "\n");
					exec.exec(command + " " + theFile, null, work);
				} else {
					File log = new File(root + File.separator + "atacs.log");
					BufferedReader input = new BufferedReader(new FileReader(log));
					String line = null;
					JTextArea messageArea = new JTextArea();
					while ((line = input.readLine()) != null) {
						messageArea.append(line);
						messageArea.append(System.getProperty("line.separator"));
					}
					input.close();
					messageArea.setLineWrap(true);
					messageArea.setWrapStyleWord(true);
					messageArea.setEditable(false);
					JScrollPane scrolls = new JScrollPane();
					scrolls.setMinimumSize(new Dimension(500, 500));
					scrolls.setPreferredSize(new Dimension(500, 500));
					scrolls.setViewportView(messageArea);
					JOptionPane.showMessageDialog(frame, scrolls, "Log", JOptionPane.INFORMATION_MESSAGE);
				}
			} else if (tree.getFile().length() >= 4
					&& tree.getFile().substring(tree.getFile().length() - 4).equals(".vhd")) {
				try {
					String vhdFile = tree.getFile();
					if (new File(vhdFile).exists()) {
						File vhdlAmsFile = new File(vhdFile);
						BufferedReader input = new BufferedReader(new FileReader(vhdlAmsFile));
						String line = null;
						JTextArea messageArea = new JTextArea();
						while ((line = input.readLine()) != null) {
							messageArea.append(line);
							messageArea.append(System.getProperty("line.separator"));
						}
						input.close();
						messageArea.setLineWrap(true);
						messageArea.setWrapStyleWord(true);
						messageArea.setEditable(false);
						JScrollPane scrolls = new JScrollPane();
						scrolls.setMinimumSize(new Dimension(800, 500));
						scrolls.setPreferredSize(new Dimension(800, 500));
						scrolls.setViewportView(messageArea);
						JOptionPane.showMessageDialog(frame, scrolls, "VHDL Model", JOptionPane.INFORMATION_MESSAGE);
					} else {
						JOptionPane.showMessageDialog(frame, "VHDL model does not exist.", "Error",
								JOptionPane.ERROR_MESSAGE);
					}
				} catch (Exception e1) {
					JOptionPane.showMessageDialog(frame, "Unable to view VHDL model.", "Error",
							JOptionPane.ERROR_MESSAGE);
				}
			} else if (tree.getFile().length() >= 5
					&& tree.getFile().substring(tree.getFile().length() - 5).equals(".vams")) {
				try {
					String vamsFileName = tree.getFile();
					if (new File(vamsFileName).exists()) {
						File vamsFile = new File(vamsFileName);
						BufferedReader input = new BufferedReader(new FileReader(vamsFile));
						String line = null;
						JTextArea messageArea = new JTextArea();
						while ((line = input.readLine()) != null) {
							messageArea.append(line);
							messageArea.append(System.getProperty("line.separator"));
						}
						input.close();
						messageArea.setLineWrap(true);
						messageArea.setWrapStyleWord(true);
						messageArea.setEditable(false);
						JScrollPane scrolls = new JScrollPane();
						scrolls.setMinimumSize(new Dimension(800, 500));
						scrolls.setPreferredSize(new Dimension(800, 500));
						scrolls.setViewportView(messageArea);
						JOptionPane.showMessageDialog(frame, scrolls, "Verilog-AMS Model",
								JOptionPane.INFORMATION_MESSAGE);
					} else {
						JOptionPane.showMessageDialog(frame, "Verilog-AMS model does not exist.", "Error",
								JOptionPane.ERROR_MESSAGE);
					}
				} catch (Exception e1) {
					JOptionPane.showMessageDialog(frame, "Unable to view Verilog-AMS model.", "Error",
							JOptionPane.ERROR_MESSAGE);
				}
			} else if (tree.getFile().length() >= 3
					&& tree.getFile().substring(tree.getFile().length() - 3).equals(".sv")) {
				try {
					String svFileName = tree.getFile();
					if (new File(svFileName).exists()) {
						File svFile = new File(svFileName);
						BufferedReader input = new BufferedReader(new FileReader(svFile));
						String line = null;
						JTextArea messageArea = new JTextArea();
						while ((line = input.readLine()) != null) {
							messageArea.append(line);
							messageArea.append(System.getProperty("line.separator"));
						}
						input.close();
						messageArea.setLineWrap(true);
						messageArea.setWrapStyleWord(true);
						messageArea.setEditable(false);
						JScrollPane scrolls = new JScrollPane();
						scrolls.setMinimumSize(new Dimension(800, 500));
						scrolls.setPreferredSize(new Dimension(800, 500));
						scrolls.setViewportView(messageArea);
						JOptionPane.showMessageDialog(frame, scrolls, "SystemVerilog Model",
								JOptionPane.INFORMATION_MESSAGE);
					} else {
						JOptionPane.showMessageDialog(frame, "SystemVerilog model does not exist.", "Error",
								JOptionPane.ERROR_MESSAGE);
					}
				} catch (Exception e1) {
					JOptionPane.showMessageDialog(frame, "Unable to view SystemVerilog model.", "Error",
							JOptionPane.ERROR_MESSAGE);
				}
			} else if (tree.getFile().length() >= 4
					&& tree.getFile().substring(tree.getFile().length() - 4).equals(".csp")) {
				String filename = GlobalConstants.getFilename(tree.getFile());
				String cmd = "atacs -lcslllodpl " + filename;
				File work = new File(root);
				Runtime exec = Runtime.getRuntime();
				Process view = exec.exec(cmd, null, work);
				log.addText("Executing:\n" + cmd);
				view.waitFor();
				String[] findTheFile = filename.split("\\.");
				// String directory = "";
				String theFile = findTheFile[0] + ".dot";
				if (new File(root + File.separator + theFile).exists()) {
					Preferences biosimrc = Preferences.userRoot();
					String command = biosimrc.get("biosim.general.graphviz", "");
					log.addText(command + " " + root + theFile + "\n");
					exec.exec(command + " " + theFile, null, work);
				} else {
					File log = new File(root + File.separator + "atacs.log");
					BufferedReader input = new BufferedReader(new FileReader(log));
					String line = null;
					JTextArea messageArea = new JTextArea();
					while ((line = input.readLine()) != null) {
						messageArea.append(line);
						messageArea.append(System.getProperty("line.separator"));
					}
					input.close();
					messageArea.setLineWrap(true);
					messageArea.setWrapStyleWord(true);
					messageArea.setEditable(false);
					JScrollPane scrolls = new JScrollPane();
					scrolls.setMinimumSize(new Dimension(500, 500));
					scrolls.setPreferredSize(new Dimension(500, 500));
					scrolls.setViewportView(messageArea);
					JOptionPane.showMessageDialog(frame, scrolls, "Log", JOptionPane.INFORMATION_MESSAGE);
				}
			} else if (tree.getFile().length() >= 4
					&& tree.getFile().substring(tree.getFile().length() - 4).equals(".hse")) {
				String filename = GlobalConstants.getFilename(tree.getFile());
				String cmd = "atacs -lhslllodpl " + filename;
				File work = new File(root);
				Runtime exec = Runtime.getRuntime();
				Process view = exec.exec(cmd, null, work);
				log.addText("Executing:\n" + cmd);
				view.waitFor();
				String[] findTheFile = filename.split("\\.");
				// String directory = "";
				String theFile = findTheFile[0] + ".dot";
				if (new File(root + File.separator + theFile).exists()) {
					Preferences biosimrc = Preferences.userRoot();
					String command = biosimrc.get("biosim.general.graphviz", "");
					log.addText(command + " " + root + theFile + "\n");
					exec.exec(command + " " + theFile, null, work);
				} else {
					File log = new File(root + File.separator + "atacs.log");
					BufferedReader input = new BufferedReader(new FileReader(log));
					String line = null;
					JTextArea messageArea = new JTextArea();
					while ((line = input.readLine()) != null) {
						messageArea.append(line);
						messageArea.append(System.getProperty("line.separator"));
					}
					input.close();
					messageArea.setLineWrap(true);
					messageArea.setWrapStyleWord(true);
					messageArea.setEditable(false);
					JScrollPane scrolls = new JScrollPane();
					scrolls.setMinimumSize(new Dimension(500, 500));
					scrolls.setPreferredSize(new Dimension(500, 500));
					scrolls.setViewportView(messageArea);
					JOptionPane.showMessageDialog(frame, scrolls, "Log", JOptionPane.INFORMATION_MESSAGE);
				}
			} else if (tree.getFile().length() >= 4
					&& tree.getFile().substring(tree.getFile().length() - 4).equals(".unc")) {
				String filename = GlobalConstants.getFilename(tree.getFile());
				String cmd = "atacs -lxodps " + filename;
				File work = new File(root);
				Runtime exec = Runtime.getRuntime();
				Process view = exec.exec(cmd, null, work);
				log.addText("Executing:\n" + cmd);
				view.waitFor();
				String[] findTheFile = filename.split("\\.");
				// String directory = "";
				String theFile = findTheFile[0] + ".dot";
				if (new File(root + File.separator + theFile).exists()) {
					Preferences biosimrc = Preferences.userRoot();
					String command = biosimrc.get("biosim.general.graphviz", "");
					log.addText(command + " " + root + theFile + "\n");
					exec.exec(command + " " + theFile, null, work);
				} else {
					File log = new File(root + File.separator + "atacs.log");
					BufferedReader input = new BufferedReader(new FileReader(log));
					String line = null;
					JTextArea messageArea = new JTextArea();
					while ((line = input.readLine()) != null) {
						messageArea.append(line);
						messageArea.append(System.getProperty("line.separator"));
					}
					input.close();
					messageArea.setLineWrap(true);
					messageArea.setWrapStyleWord(true);
					messageArea.setEditable(false);
					JScrollPane scrolls = new JScrollPane();
					scrolls.setMinimumSize(new Dimension(500, 500));
					scrolls.setPreferredSize(new Dimension(500, 500));
					scrolls.setViewportView(messageArea);
					JOptionPane.showMessageDialog(frame, scrolls, "Log", JOptionPane.INFORMATION_MESSAGE);
				}
			} else if (tree.getFile().length() >= 4
					&& tree.getFile().substring(tree.getFile().length() - 4).equals(".rsg")) {
				String filename = GlobalConstants.getFilename(tree.getFile());
				String cmd = "atacs -lsodps " + filename;
				File work = new File(root);
				Runtime exec = Runtime.getRuntime();
				Process view = exec.exec(cmd, null, work);
				log.addText("Executing:\n" + cmd);
				view.waitFor();
				String[] findTheFile = filename.split("\\.");
				// String directory = "";
				String theFile = findTheFile[0] + ".dot";
				if (new File(root + File.separator + theFile).exists()) {
					Preferences biosimrc = Preferences.userRoot();
					String command = biosimrc.get("biosim.general.graphviz", "");
					log.addText(command + " " + root + theFile + "\n");
					exec.exec(command + " " + theFile, null, work);
				} else {
					File log = new File(root + File.separator + "atacs.log");
					BufferedReader input = new BufferedReader(new FileReader(log));
					String line = null;
					JTextArea messageArea = new JTextArea();
					while ((line = input.readLine()) != null) {
						messageArea.append(line);
						messageArea.append(System.getProperty("line.separator"));
					}
					input.close();
					messageArea.setLineWrap(true);
					messageArea.setWrapStyleWord(true);
					messageArea.setEditable(false);
					JScrollPane scrolls = new JScrollPane();
					scrolls.setMinimumSize(new Dimension(500, 500));
					scrolls.setPreferredSize(new Dimension(500, 500));
					scrolls.setViewportView(messageArea);
					JOptionPane.showMessageDialog(frame, scrolls, "Log", JOptionPane.INFORMATION_MESSAGE);
				}
			}
		} catch (IOException e1) {
			e1.printStackTrace();
			JOptionPane.showMessageDialog(frame, "File cannot be read", "Error", JOptionPane.ERROR_MESSAGE);
		} catch (InterruptedException e2) {
			e2.printStackTrace();
		}
	}

	private void openLPN() {
		try {
			String filename = tree.getFile();
			String directory = "";
			String theFile = "";
			if (filename.lastIndexOf('/') >= 0) {
				directory = filename.substring(0, filename.lastIndexOf('/') + 1);
				theFile = filename.substring(filename.lastIndexOf('/') + 1);
			}
			if (filename.lastIndexOf('\\') >= 0) {
				directory = filename.substring(0, filename.lastIndexOf('\\') + 1);
				theFile = filename.substring(filename.lastIndexOf('\\') + 1);
			}
			LPN lhpn = new LPN();
			lhpn.addObserver(this);
			File work = new File(directory);
			int i = getTab(theFile);
			if (i != -1) {
				tab.setSelectedIndex(i);
			} else {
				LHPNEditor editor = new LHPNEditor(work.getAbsolutePath(), theFile, lhpn, this);
				addTab(theFile, editor, "LHPN Editor");
			}
		} catch (Exception e1) {
			e1.printStackTrace();
			JOptionPane.showMessageDialog(frame, "Unable to view this LPN file.", "Error", JOptionPane.ERROR_MESSAGE);
		}
	}

	/**
	 * Prompts the user to save work that has been done.
	 */
	public int save(int index, int autosave) {
		if (tab.getComponentAt(index).getName().contains(("Model Editor"))
				|| tab.getComponentAt(index).getName().contains("LHPN")) {
			if (tab.getComponentAt(index) instanceof ModelEditor) {
				ModelEditor editor = (ModelEditor) tab.getComponentAt(index);
				if (editor.isDirty()) {
					if (autosave == 0) {
						int value = JOptionPane.showOptionDialog(frame,
								"Do you want to save changes to " + getTitleAt(index) + "?", "Save Changes",
								JOptionPane.YES_NO_CANCEL_OPTION, JOptionPane.PLAIN_MESSAGE, null, OPTIONS, OPTIONS[0]);
						if (value == YES_OPTION) {
							editor.save(false);
							return 1;
						} else if (value == NO_OPTION) {
							return 1;
						} else if (value == CANCEL_OPTION) {
							return 0;
						} else if (value == YES_TO_ALL_OPTION) {
							editor.save(false);
							return 2;
						} else if (value == NO_TO_ALL_OPTION) {
							return 3;
						}
					} else if (autosave == 1) {
						editor.save(false);
						return 2;
					} else {
						return 3;
					}
				}
			} else if (tab.getComponentAt(index) instanceof LHPNEditor) {
				LHPNEditor editor = (LHPNEditor) tab.getComponentAt(index);
				if (editor.isDirty()) {
					if (autosave == 0) {
						int value = JOptionPane.showOptionDialog(frame,
								"Do you want to save changes to " + getTitleAt(index) + "?", "Save Changes",
								JOptionPane.YES_NO_CANCEL_OPTION, JOptionPane.PLAIN_MESSAGE, null, OPTIONS, OPTIONS[0]);
						if (value == YES_OPTION) {
							editor.save();
							return 1;
						} else if (value == NO_OPTION) {
							return 1;
						} else if (value == CANCEL_OPTION) {
							return 0;
						} else if (value == YES_TO_ALL_OPTION) {
							editor.save();
							return 2;
						} else if (value == NO_TO_ALL_OPTION) {
							return 3;
						}
					} else if (autosave == 1) {
						editor.save();
						return 2;
					} else {
						return 3;
					}
				}
			}
			if (autosave == 0) {
				return 1;
			} else if (autosave == 1) {
				return 2;
			} else {
				return 3;
			}
		} else if (tab.getComponentAt(index).getName().contains("Graph")
				|| tab.getComponentAt(index).getName().equals("Histogram")) {
			if (tab.getComponentAt(index) instanceof Graph) {
				if (((Graph) tab.getComponentAt(index)).hasChanged()) {
					if (autosave == 0) {
						int value = JOptionPane.showOptionDialog(frame,
								"Do you want to save changes to " + getTitleAt(index) + "?", "Save Changes",
								JOptionPane.YES_NO_CANCEL_OPTION, JOptionPane.PLAIN_MESSAGE, null, OPTIONS, OPTIONS[0]);
						if (value == YES_OPTION) {
							((Graph) tab.getComponentAt(index)).save();
							return 1;
						} else if (value == NO_OPTION) {
							return 1;
						} else if (value == CANCEL_OPTION) {
							return 0;
						} else if (value == YES_TO_ALL_OPTION) {
							((Graph) tab.getComponentAt(index)).save();
							return 2;
						} else if (value == NO_TO_ALL_OPTION) {
							return 3;
						}
					} else if (autosave == 1) {
						((Graph) tab.getComponentAt(index)).save();
						return 2;
					} else {
						return 3;
					}
				}
			}
			if (autosave == 0) {
				return 1;
			} else if (autosave == 1) {
				return 2;
			} else {
				return 3;
			}
		}

		else {
			if (tab.getComponentAt(index) instanceof JTabbedPane) {
				if (tab.getComponentAt(index) instanceof SynthesisView) {
					SynthesisView synthView = (SynthesisView) tab.getComponentAt(index);
					Set<Integer> saveIndices = new HashSet<Integer>();
					for (int i = 0; i < synthView.getTabCount(); i++) {
						JPanel synthTab = (JPanel) synthView.getComponentAt(i);
						if (synthView.tabChanged(i)) {
							if (autosave == 0) {
								int value = JOptionPane.showOptionDialog(frame,
										"Do you want to save changes to " + synthTab.getName() + "?", "Save Changes",
										JOptionPane.YES_NO_CANCEL_OPTION, JOptionPane.PLAIN_MESSAGE, null, OPTIONS,
										OPTIONS[0]);
								if (value == YES_OPTION) {
									saveIndices.add(i);
								} else if (value == CANCEL_OPTION) {
									return 0;
								} else if (value == YES_TO_ALL_OPTION) {
									saveIndices.add(i);
									autosave = 1;
								} else if (value == NO_TO_ALL_OPTION) {
									autosave = 2;
								}
							} else if (autosave == 1) {
								saveIndices.add(i);
							}
						}
					}
					synthView.saveTabs(saveIndices);
				} else {
					for (int i = 0; i < ((JTabbedPane) tab.getComponentAt(index)).getTabCount(); i++) {
						if (((JTabbedPane) tab.getComponentAt(index)).getComponentAt(i).getName() != null) {
							if (((JTabbedPane) tab.getComponentAt(index)).getComponentAt(i).getName()
									.equals("Simulate")) {
								if (((AnalysisView) ((JTabbedPane) tab.getComponentAt(index)).getComponent(i))
										.hasChanged()) {
									if (autosave == 0) {
										int value = JOptionPane.showOptionDialog(frame,
												"Do you want to save simulation option changes for " + getTitleAt(index)
												+ "?",
												"Save Changes", JOptionPane.YES_NO_CANCEL_OPTION,
												JOptionPane.PLAIN_MESSAGE, null, OPTIONS, OPTIONS[0]);
										if (value == YES_OPTION) {
											((AnalysisView) ((JTabbedPane) tab.getComponentAt(index)).getComponent(i))
											.save();
										} else if (value == CANCEL_OPTION) {
											return 0;
										} else if (value == YES_TO_ALL_OPTION) {
											((AnalysisView) ((JTabbedPane) tab.getComponentAt(index)).getComponent(i))
											.save();
											autosave = 1;
										} else if (value == NO_TO_ALL_OPTION) {
											autosave = 2;
										}
									} else if (autosave == 1) {
										((AnalysisView) ((JTabbedPane) tab.getComponentAt(index)).getComponent(i))
										.save();
									}
								}
							} else if (((JTabbedPane) tab.getComponentAt(index))
									.getComponent(i) instanceof MovieContainer) {
								if (((MovieContainer) ((JTabbedPane) tab.getComponentAt(index)).getComponent(i))
										.getGCM2SBMLEditor().isDirty()
										|| ((MovieContainer) ((JTabbedPane) tab.getComponentAt(index)).getComponent(i))
										.getIsDirty()) {
									if (autosave == 0) {
										int value = JOptionPane.showOptionDialog(frame,
												"Do you want to save parameter changes for " + getTitleAt(index) + "?",
												"Save Changes", JOptionPane.YES_NO_CANCEL_OPTION,
												JOptionPane.PLAIN_MESSAGE, null, OPTIONS, OPTIONS[0]);
										if (value == YES_OPTION) {
											((MovieContainer) ((JTabbedPane) tab.getComponentAt(index)).getComponent(i))
											.savePreferences();
										} else if (value == CANCEL_OPTION) {
											return 0;
										} else if (value == YES_TO_ALL_OPTION) {
											((MovieContainer) ((JTabbedPane) tab.getComponentAt(index)).getComponent(i))
											.savePreferences();
											autosave = 1;
										} else if (value == NO_TO_ALL_OPTION) {
											autosave = 2;
										}
									} else if (autosave == 1) {
										((MovieContainer) ((JTabbedPane) tab.getComponentAt(index)).getComponent(i))
										.savePreferences();
									}
								}
							} else if (((JTabbedPane) tab.getComponentAt(index)).getComponent(i).getName()
									.equals("Learn Options")) {
								if (((JTabbedPane) tab.getComponentAt(index)).getComponent(i) instanceof LearnView) {
									if (((LearnView) ((JTabbedPane) tab.getComponentAt(index)).getComponent(i))
											.hasChanged()) {
										if (autosave == 0) {
											int value = JOptionPane.showOptionDialog(frame,
													"Do you want to save learn option changes for " + getTitleAt(index)
													+ "?",
													"Save Changes", JOptionPane.YES_NO_CANCEL_OPTION,
													JOptionPane.PLAIN_MESSAGE, null, OPTIONS, OPTIONS[0]);
											if (value == YES_OPTION) {
												if (((JTabbedPane) tab.getComponentAt(index))
														.getComponent(i) instanceof LearnView) {
													((LearnView) ((JTabbedPane) tab.getComponentAt(index))
															.getComponent(i)).save();
												}
											} else if (value == CANCEL_OPTION) {
												return 0;
											} else if (value == YES_TO_ALL_OPTION) {
												if (((JTabbedPane) tab.getComponentAt(index))
														.getComponent(i) instanceof LearnView) {
													((LearnView) ((JTabbedPane) tab.getComponentAt(index))
															.getComponent(i)).save();
												}
												autosave = 1;
											} else if (value == NO_TO_ALL_OPTION) {
												autosave = 2;
											}
										} else if (autosave == 1) {
											if (((JTabbedPane) tab.getComponentAt(index))
													.getComponent(i) instanceof LearnView) {
												((LearnView) ((JTabbedPane) tab.getComponentAt(index)).getComponent(i))
												.save();
											}
										}
									}
								}
							} else if (((JTabbedPane) tab.getComponentAt(index)).getComponent(i).getName()
									.equals("Data Manager")) {
								if (((JTabbedPane) tab.getComponentAt(index)).getComponent(i) instanceof DataManager) {
									((DataManager) ((JTabbedPane) tab.getComponentAt(index)).getComponent(i))
									.saveChanges(getTitleAt(index));
								}
							} else if (((JTabbedPane) tab.getComponentAt(index)).getComponent(i).getName()
									.contains("Graph")) {
								if (((JTabbedPane) tab.getComponentAt(index)).getComponent(i) instanceof Graph) {
									if (((Graph) ((JTabbedPane) tab.getComponentAt(index)).getComponent(i))
											.hasChanged()) {
										if (autosave == 0) {
											int value = JOptionPane.showOptionDialog(frame,
													"Do you want to save graph changes for " + getTitleAt(index) + "?",
													"Save Changes", JOptionPane.YES_NO_CANCEL_OPTION,
													JOptionPane.PLAIN_MESSAGE, null, OPTIONS, OPTIONS[0]);
											if (value == YES_OPTION) {
												if (((JTabbedPane) tab.getComponentAt(index))
														.getComponent(i) instanceof Graph) {
													Graph g = ((Graph) ((JTabbedPane) tab.getComponentAt(index))
															.getComponent(i));
													g.save();
												}
											} else if (value == CANCEL_OPTION) {
												return 0;
											} else if (value == YES_TO_ALL_OPTION) {
												if (((JTabbedPane) tab.getComponentAt(index))
														.getComponent(i) instanceof Graph) {
													Graph g = ((Graph) ((JTabbedPane) tab.getComponentAt(index))
															.getComponent(i));
													g.save();
												}
												autosave = 1;
											} else if (value == NO_TO_ALL_OPTION) {
												autosave = 2;
											}
										} else if (autosave == 1) {
											if (((JTabbedPane) tab.getComponentAt(index))
													.getComponent(i) instanceof Graph) {
												Graph g = ((Graph) ((JTabbedPane) tab.getComponentAt(index))
														.getComponent(i));
												g.save();
											}
										}
									}
								}
							}
						}
					}
				}
			} else if (tab.getComponentAt(index) instanceof JPanel) {
				if ((tab.getComponentAt(index)).getName().equals("Synthesis")) {
					Component[] array = ((JPanel) tab.getComponentAt(index)).getComponents();
					if (array[0] instanceof SynthesisViewATACS) {
						if (((SynthesisViewATACS) array[0]).hasChanged()) {
							if (autosave == 0) {
								int value = JOptionPane.showOptionDialog(frame,
										"Do you want to save synthesis option changes for " + getTitleAt(index) + "?",
										"Save Changes", JOptionPane.YES_NO_CANCEL_OPTION, JOptionPane.PLAIN_MESSAGE,
										null, OPTIONS, OPTIONS[0]);
								if (value == YES_OPTION) {
									if (array[0] instanceof SynthesisViewATACS) {
										((SynthesisViewATACS) array[0]).save();
									}
								} else if (value == CANCEL_OPTION) {
									return 0;
								} else if (value == YES_TO_ALL_OPTION) {
									if (array[0] instanceof SynthesisViewATACS) {
										((SynthesisViewATACS) array[0]).save();
									}
									autosave = 1;
								} else if (value == NO_TO_ALL_OPTION) {
									autosave = 2;
								}
							} else if (autosave == 1) {
								if (array[0] instanceof SynthesisViewATACS) {
									((SynthesisViewATACS) array[0]).save();
								}
							}
						}
					}
				} else if (tab.getComponentAt(index).getName().equals("Verification")) {
					Component[] array = ((JPanel) tab.getComponentAt(index)).getComponents();
					if (array[0] instanceof VerificationView) {
						if (((VerificationView) array[0]).hasChanged()) {
							if (autosave == 0) {
								int value = JOptionPane.showOptionDialog(frame,
										"Do you want to save verification option changes for " + getTitleAt(index)
										+ "?",
										"Save Changes", JOptionPane.YES_NO_CANCEL_OPTION, JOptionPane.PLAIN_MESSAGE,
										null, OPTIONS, OPTIONS[0]);
								if (value == YES_OPTION) {
									((VerificationView) array[0]).save();
								} else if (value == CANCEL_OPTION) {
									return 0;
								} else if (value == YES_TO_ALL_OPTION) {
									((VerificationView) array[0]).save();
									autosave = 1;
								} else if (value == NO_TO_ALL_OPTION) {
									autosave = 2;
								}
							} else if (autosave == 1) {
								((VerificationView) array[0]).save();
							}
						}
					}
				}
			}
			if (autosave == 0) {
				return 1;
			} else if (autosave == 1) {
				return 2;
			} else {
				return 3;
			}
		}
	}

	@Override
	public void mousePressed(MouseEvent e) {
		executePopupMenu(e);
	}

	@Override
	public void mouseReleased(MouseEvent e) {
		executePopupMenu(e);
	}

	public void executePopupMenu(MouseEvent e) {
		if (e.getSource() instanceof JTree && tree.getFile() != null && e.isPopupTrigger()) {
			// frame.getGlassPane().setVisible(false);
			popup.removeAll();
			if (tree.getFile().endsWith(".sbml") || tree.getFile().endsWith(".xml")) {
				JMenuItem create = new JMenuItem("Create Analysis View");
				create.addActionListener(this);
				create.addMouseListener(this);
				create.setActionCommand("createAnalysis");
				JMenuItem createSynthesis = new JMenuItem("Create Synthesis View");
				createSynthesis.addActionListener(this);
				createSynthesis.addMouseListener(this);
				createSynthesis.setActionCommand("createSynthesis");
				JMenuItem createLearn = new JMenuItem("Create Learn View");
				createLearn.addActionListener(this);
				createLearn.addMouseListener(this);
				createLearn.setActionCommand("createLearn");
				JMenuItem createVerification = new JMenuItem("Create Verification View");
				createVerification.addActionListener(this);
				createVerification.addMouseListener(this);
				createVerification.setActionCommand("createVerify");
				JMenuItem edit = new JMenuItem("View/Edit (graphical)");
				edit.addActionListener(this);
				edit.addMouseListener(this);
				edit.setActionCommand("modelEditor");
				JMenuItem editText = new JMenuItem("View/Edit (tabular)");
				editText.addActionListener(this);
				editText.addMouseListener(this);
				editText.setActionCommand("modelTextEditor");
				JMenuItem delete = new JMenuItem("Delete");
				delete.addActionListener(this);
				delete.addMouseListener(this);
				delete.setActionCommand("delete");
				JMenuItem copy = new JMenuItem("Copy");
				copy.addActionListener(this);
				copy.addMouseListener(this);
				copy.setActionCommand("copy");
				JMenuItem rename = new JMenuItem("Rename");
				rename.addActionListener(this);
				rename.addMouseListener(this);
				rename.setActionCommand("rename");
				popup.add(create);
				popup.add(createSynthesis);
				popup.add(createLearn);
				popup.add(createVerification);
				popup.addSeparator();
				popup.add(edit);
				popup.add(editText);
				popup.add(copy);
				popup.add(rename);
				popup.add(delete);
			} 
			else if (tree.getFile().endsWith(".vhd")) {
				JMenuItem createSynthesis = new JMenuItem("Create Synthesis View");
				createSynthesis.addActionListener(this);
				createSynthesis.addMouseListener(this);
				createSynthesis.setActionCommand("createSynthesis");
				JMenuItem createAnalysis = new JMenuItem("Create Analysis View");
				createAnalysis.addActionListener(this);
				createAnalysis.addMouseListener(this);
				createAnalysis.setActionCommand("createAnalysis");
				JMenuItem createLearn = new JMenuItem("Create Learn View");
				createLearn.addActionListener(this);
				createLearn.addMouseListener(this);
				createLearn.setActionCommand("createLearn");
				JMenuItem createVerification = new JMenuItem("Create Verification View");
				createVerification.addActionListener(this);
				createVerification.addMouseListener(this);
				createVerification.setActionCommand("createVerify");
				JMenuItem viewModel = new JMenuItem("View Model");
				viewModel.addActionListener(this);
				viewModel.addMouseListener(this);
				viewModel.setActionCommand("viewModel");
				JMenuItem delete = new JMenuItem("Delete");
				delete.addActionListener(this);
				delete.addMouseListener(this);
				delete.setActionCommand("delete");
				JMenuItem copy = new JMenuItem("Copy");
				copy.addActionListener(this);
				copy.addMouseListener(this);
				copy.setActionCommand("copy");
				JMenuItem rename = new JMenuItem("Rename");
				rename.addActionListener(this);
				rename.addMouseListener(this);
				rename.setActionCommand("rename");
				popup.add(createSynthesis);
				popup.add(createVerification);
				popup.addSeparator();
				popup.add(viewModel);
				popup.addSeparator();
				popup.add(copy);
				popup.add(rename);
				popup.add(delete);
			} else if (tree.getFile().endsWith(".vams")) {
				JMenuItem viewModel = new JMenuItem("View Model");
				viewModel.addActionListener(this);
				viewModel.addMouseListener(this);
				viewModel.setActionCommand("viewModel");
				JMenuItem delete = new JMenuItem("Delete");
				delete.addActionListener(this);
				delete.addMouseListener(this);
				delete.setActionCommand("delete");
				JMenuItem copy = new JMenuItem("Copy");
				copy.addActionListener(this);
				copy.addMouseListener(this);
				copy.setActionCommand("copy");
				JMenuItem rename = new JMenuItem("Rename");
				rename.addActionListener(this);
				rename.addMouseListener(this);
				rename.setActionCommand("rename");
			} else if (tree.getFile().endsWith(".sv")) {
				JMenuItem viewModel = new JMenuItem("View Model");
				viewModel.addActionListener(this);
				viewModel.addMouseListener(this);
				viewModel.setActionCommand("viewModel");
				JMenuItem delete = new JMenuItem("Delete");
				delete.addActionListener(this);
				delete.addMouseListener(this);
				delete.setActionCommand("delete");
				JMenuItem copy = new JMenuItem("Copy");
				copy.addActionListener(this);
				copy.addMouseListener(this);
				copy.setActionCommand("copy");
				JMenuItem rename = new JMenuItem("Rename");
				rename.addActionListener(this);
				rename.addMouseListener(this);
				rename.setActionCommand("rename");
			} else if (tree.getFile().endsWith(".g")) {
				JMenuItem createSynthesis = new JMenuItem("Create Synthesis View");
				createSynthesis.addActionListener(this);
				createSynthesis.addMouseListener(this);
				createSynthesis.setActionCommand("createSynthesis");
				JMenuItem createAnalysis = new JMenuItem("Create Analysis View");
				createAnalysis.addActionListener(this);
				createAnalysis.addMouseListener(this);
				createAnalysis.setActionCommand("createAnalysis");
				JMenuItem createLearn = new JMenuItem("Create Learn View");
				createLearn.addActionListener(this);
				createLearn.addMouseListener(this);
				createLearn.setActionCommand("createLearn");
				JMenuItem createVerification = new JMenuItem("Create Verification View");
				createVerification.addActionListener(this);
				createVerification.addMouseListener(this);
				createVerification.setActionCommand("createVerify");
				JMenuItem viewModel = new JMenuItem("View Model");
				viewModel.addActionListener(this);
				viewModel.addMouseListener(this);
				viewModel.setActionCommand("viewModel");
				JMenuItem delete = new JMenuItem("Delete");
				delete.addActionListener(this);
				delete.addMouseListener(this);
				delete.setActionCommand("delete");
				JMenuItem copy = new JMenuItem("Copy");
				copy.addActionListener(this);
				copy.addMouseListener(this);
				copy.setActionCommand("copy");
				JMenuItem rename = new JMenuItem("Rename");
				rename.addActionListener(this);
				rename.addMouseListener(this);
				rename.setActionCommand("rename");
				popup.add(createSynthesis);
				popup.add(createVerification);
				popup.addSeparator();
				popup.add(viewModel);
				popup.addSeparator();
				popup.add(copy);
				popup.add(rename);
				popup.add(delete);
			} else if (tree.getFile().endsWith(".lpn")) {
				JMenuItem createSynthesis = new JMenuItem("Create Synthesis View");
				createSynthesis.addActionListener(this);
				createSynthesis.addMouseListener(this);
				createSynthesis.setActionCommand("createSynthesis");
				JMenuItem createAnalysis = new JMenuItem("Create Analysis View");
				createAnalysis.addActionListener(this);
				createAnalysis.addMouseListener(this);
				createAnalysis.setActionCommand("createAnalysis");
				JMenuItem createLearn = new JMenuItem("Create Learn View");
				createLearn.addActionListener(this);
				createLearn.addMouseListener(this);
				createLearn.setActionCommand("createLearn");
				JMenuItem createVerification = new JMenuItem("Create Verification View");
				createVerification.addActionListener(this);
				createVerification.addMouseListener(this);
				createVerification.setActionCommand("createVerify");
				JMenuItem convertToSBML = new JMenuItem("Convert To SBML");
				convertToSBML.addActionListener(this);
				convertToSBML.addMouseListener(this);
				convertToSBML.setActionCommand("convertToSBML");
				JMenuItem convertToVerilog = new JMenuItem("Save as Verilog");
				convertToVerilog.addActionListener(this);
				convertToVerilog.addMouseListener(this);
				convertToVerilog.setActionCommand("convertToVerilog");
				JMenuItem viewModel = new JMenuItem("View Model");
				viewModel.addActionListener(this);
				viewModel.addMouseListener(this);
				viewModel.setActionCommand("viewModel");
				JMenuItem view = new JMenuItem("View/Edit");
				view.addActionListener(this);
				view.addMouseListener(this);
				view.setActionCommand("openLPN");
				JMenuItem delete = new JMenuItem("Delete");
				delete.addActionListener(this);
				delete.addMouseListener(this);
				delete.setActionCommand("delete");
				JMenuItem copy = new JMenuItem("Copy");
				copy.addActionListener(this);
				copy.addMouseListener(this);
				copy.setActionCommand("copy");
				JMenuItem rename = new JMenuItem("Rename");
				rename.addActionListener(this);
				rename.addMouseListener(this);
				rename.setActionCommand("rename");
				popup.add(createSynthesis);
				popup.add(createAnalysis);
				popup.add(createVerification);
				popup.add(convertToVerilog);
				popup.add(viewModel);
				popup.addSeparator();
				popup.add(view);
				popup.add(copy);
				popup.add(rename);
				popup.add(delete);
			}

			else if (tree.getFile().endsWith(".prop")) {

				JMenuItem convertToLPN = new JMenuItem("Convert To LPN");
				convertToLPN.addActionListener(this);
				convertToLPN.addMouseListener(this);
				convertToLPN.setActionCommand("convertToLPN");

				JMenuItem view = new JMenuItem("View/Edit");
				view.addActionListener(this);
				view.addMouseListener(this);
				view.setActionCommand("openLPN");
				JMenuItem delete = new JMenuItem("Delete");
				delete.addActionListener(this);
				delete.addMouseListener(this);
				delete.setActionCommand("delete");
				JMenuItem copy = new JMenuItem("Copy");
				copy.addActionListener(this);
				copy.addMouseListener(this);
				copy.setActionCommand("copy");
				JMenuItem rename = new JMenuItem("Rename");
				rename.addActionListener(this);
				rename.addMouseListener(this);
				rename.setActionCommand("rename");
				popup.addSeparator();
				popup.add(view);
				popup.add(copy);
				popup.add(rename);
				popup.add(delete);
				popup.add(convertToLPN);
			}

			else if (tree.getFile().endsWith(".s")) {
				JMenuItem createAnalysis = new JMenuItem("Create Analysis View");
				createAnalysis.addActionListener(this);
				createAnalysis.addMouseListener(this);
				createAnalysis.setActionCommand("createAnalysis");
				JMenuItem createVerification = new JMenuItem("Create Verification View");
				createVerification.addActionListener(this);
				createVerification.addMouseListener(this);
				createVerification.setActionCommand("createVerify");
				JMenuItem delete = new JMenuItem("Delete");
				delete.addActionListener(this);
				delete.addMouseListener(this);
				delete.setActionCommand("delete");
				JMenuItem copy = new JMenuItem("Copy");
				copy.addActionListener(this);
				copy.addMouseListener(this);
				copy.setActionCommand("copy");
				JMenuItem rename = new JMenuItem("Rename");
				rename.addActionListener(this);
				rename.addMouseListener(this);
				rename.setActionCommand("rename");
				popup.add(createAnalysis);
				popup.add(createVerification);
				popup.addSeparator();
				popup.add(copy);
				popup.add(rename);
				popup.add(delete);
			} else if (tree.getFile().endsWith(".inst")) {
				JMenuItem delete = new JMenuItem("Delete");
				delete.addActionListener(this);
				delete.addMouseListener(this);
				delete.setActionCommand("delete");
				JMenuItem copy = new JMenuItem("Copy");
				copy.addActionListener(this);
				copy.addMouseListener(this);
				copy.setActionCommand("copy");
				JMenuItem rename = new JMenuItem("Rename");
				rename.addActionListener(this);
				rename.addMouseListener(this);
				rename.setActionCommand("rename");
				popup.add(copy);
				popup.add(rename);
				popup.add(delete);
			} else if (tree.getFile().endsWith(".csp")) {
				JMenuItem createSynthesis = new JMenuItem("Create Synthesis View");
				createSynthesis.addActionListener(this);
				createSynthesis.addMouseListener(this);
				createSynthesis.setActionCommand("createSynthesis");
				JMenuItem createAnalysis = new JMenuItem("Create Analysis View");
				createAnalysis.addActionListener(this);
				createAnalysis.addMouseListener(this);
				createAnalysis.setActionCommand("createAnalysis");
				JMenuItem createLearn = new JMenuItem("Create Learn View");
				createLearn.addActionListener(this);
				createLearn.addMouseListener(this);
				createLearn.setActionCommand("createLearn");
				JMenuItem createVerification = new JMenuItem("Create Verification View");
				createVerification.addActionListener(this);
				createVerification.addMouseListener(this);
				createVerification.setActionCommand("createVerify");
				JMenuItem viewModel = new JMenuItem("View Model");
				viewModel.addActionListener(this);
				viewModel.addMouseListener(this);
				viewModel.setActionCommand("viewModel");
				JMenuItem delete = new JMenuItem("Delete");
				delete.addActionListener(this);
				delete.addMouseListener(this);
				delete.setActionCommand("delete");
				JMenuItem copy = new JMenuItem("Copy");
				copy.addActionListener(this);
				copy.addMouseListener(this);
				copy.setActionCommand("copy");
				JMenuItem rename = new JMenuItem("Rename");
				rename.addActionListener(this);
				rename.addMouseListener(this);
				rename.setActionCommand("rename");
				popup.add(createSynthesis);
				popup.add(createVerification);
				popup.addSeparator();
				popup.add(viewModel);
				popup.addSeparator();
				popup.add(copy);
				popup.add(rename);
				popup.add(delete);
			} else if (tree.getFile().endsWith(".hse")) {
				JMenuItem createSynthesis = new JMenuItem("Create Synthesis View");
				createSynthesis.addActionListener(this);
				createSynthesis.addMouseListener(this);
				createSynthesis.setActionCommand("createSynthesis");
				JMenuItem createAnalysis = new JMenuItem("Create Analysis View");
				createAnalysis.addActionListener(this);
				createAnalysis.addMouseListener(this);
				createAnalysis.setActionCommand("createAnalysis");
				JMenuItem createLearn = new JMenuItem("Create Learn View");
				createLearn.addActionListener(this);
				createLearn.addMouseListener(this);
				createLearn.setActionCommand("createLearn");
				JMenuItem createVerification = new JMenuItem("Create Verification View");
				createVerification.addActionListener(this);
				createVerification.addMouseListener(this);
				createVerification.setActionCommand("createVerify");
				JMenuItem viewModel = new JMenuItem("View Model");
				viewModel.addActionListener(this);
				viewModel.addMouseListener(this);
				viewModel.setActionCommand("viewModel");
				JMenuItem delete = new JMenuItem("Delete");
				delete.addActionListener(this);
				delete.addMouseListener(this);
				delete.setActionCommand("delete");
				JMenuItem copy = new JMenuItem("Copy");
				copy.addActionListener(this);
				copy.addMouseListener(this);
				copy.setActionCommand("copy");
				JMenuItem rename = new JMenuItem("Rename");
				rename.addActionListener(this);
				rename.addMouseListener(this);
				rename.setActionCommand("rename");
				popup.add(createSynthesis);
				popup.add(createVerification);
				popup.addSeparator();
				popup.add(viewModel);
				popup.addSeparator();
				popup.add(copy);
				popup.add(rename);
				popup.add(delete);
			} else if (tree.getFile().endsWith(".unc")) {
				JMenuItem createSynthesis = new JMenuItem("Create Synthesis View");
				createSynthesis.addActionListener(this);
				createSynthesis.addMouseListener(this);
				createSynthesis.setActionCommand("createSynthesis");
				JMenuItem viewModel = new JMenuItem("View Model");
				viewModel.addActionListener(this);
				viewModel.addMouseListener(this);
				viewModel.setActionCommand("viewModel");
				JMenuItem delete = new JMenuItem("Delete");
				delete.addActionListener(this);
				delete.addMouseListener(this);
				delete.setActionCommand("delete");
				JMenuItem copy = new JMenuItem("Copy");
				copy.addActionListener(this);
				copy.addMouseListener(this);
				copy.setActionCommand("copy");
				JMenuItem rename = new JMenuItem("Rename");
				rename.addActionListener(this);
				rename.addMouseListener(this);
				rename.setActionCommand("rename");
				popup.add(createSynthesis);
				popup.addSeparator();
				popup.add(viewModel);
				popup.addSeparator();
				popup.add(copy);
				popup.add(rename);
				popup.add(delete);
			} else if (tree.getFile().endsWith(".rsg")) {
				JMenuItem createSynthesis = new JMenuItem("Create Synthesis View");
				createSynthesis.addActionListener(this);
				createSynthesis.addMouseListener(this);
				createSynthesis.setActionCommand("createSynthesis");
				JMenuItem viewModel = new JMenuItem("View Model");
				viewModel.addActionListener(this);
				viewModel.addMouseListener(this);
				viewModel.setActionCommand("viewModel");
				JMenuItem delete = new JMenuItem("Delete");
				delete.addActionListener(this);
				delete.addMouseListener(this);
				delete.setActionCommand("delete");
				JMenuItem copy = new JMenuItem("Copy");
				copy.addActionListener(this);
				copy.addMouseListener(this);
				copy.setActionCommand("copy");
				JMenuItem rename = new JMenuItem("Rename");
				rename.addActionListener(this);
				rename.addMouseListener(this);
				rename.setActionCommand("rename");
				popup.add(createSynthesis);
				popup.addSeparator();
				popup.add(viewModel);
				popup.addSeparator();
				popup.add(copy);
				popup.add(rename);
				popup.add(delete);
			} else if (tree.getFile().endsWith(".grf")) {
				JMenuItem edit = new JMenuItem("View/Edit");
				edit.addActionListener(this);
				edit.addMouseListener(this);
				edit.setActionCommand("openGraph");
				JMenuItem delete = new JMenuItem("Delete");
				delete.addActionListener(this);
				delete.addMouseListener(this);
				delete.setActionCommand("delete");
				JMenuItem copy = new JMenuItem("Copy");
				copy.addActionListener(this);
				copy.addMouseListener(this);
				copy.setActionCommand("copy");
				JMenuItem rename = new JMenuItem("Rename");
				rename.addActionListener(this);
				rename.addMouseListener(this);
				rename.setActionCommand("rename");
				popup.add(edit);
				popup.add(copy);
				popup.add(rename);
				popup.add(delete);
			} else if (tree.getFile().endsWith(".prb")) {
				JMenuItem edit = new JMenuItem("View/Edit");
				edit.addActionListener(this);
				edit.addMouseListener(this);
				edit.setActionCommand("openHistogram");
				JMenuItem delete = new JMenuItem("Delete");
				delete.addActionListener(this);
				delete.addMouseListener(this);
				delete.setActionCommand("delete");
				JMenuItem copy = new JMenuItem("Copy");
				copy.addActionListener(this);
				copy.addMouseListener(this);
				copy.setActionCommand("copy");
				JMenuItem rename = new JMenuItem("Rename");
				rename.addActionListener(this);
				rename.addMouseListener(this);
				rename.setActionCommand("rename");
				popup.add(edit);
				popup.add(copy);
				popup.add(rename);
				popup.add(delete);
			} else if (new File(tree.getFile()).isDirectory() && !tree.getFile().equals(root)) {
				boolean sim = false;
				boolean synth = false;
				boolean ver = false;
				boolean learn = false;
				for (String s : new File(tree.getFile()).list()) {
					if (s.endsWith(".sim")) {
						sim = true;
					}
					if ((s.endsWith(".syn")) || s.endsWith(GlobalConstants.SBOL_SYNTH_PROPERTIES_EXTENSION)) {
						synth = true;
					}
					if (s.endsWith(".ver")) {
						ver = true;
					}
					if (s.endsWith(".lrn")) {
						learn = true;
					}
				}
				JMenuItem open;
				if (sim) {
					open = new JMenuItem("Open Analysis View");
					open.addActionListener(this);
					open.addMouseListener(this);
					open.setActionCommand("openSim");
					popup.add(open);
				} else if (synth) {
					open = new JMenuItem("Open Synthesis View");
					open.addActionListener(this);
					open.addMouseListener(this);
					open.setActionCommand("openSynth");
					popup.add(open);
				} else if (ver) {
					open = new JMenuItem("Open Verification View");
					open.addActionListener(this);
					open.addMouseListener(this);
					open.setActionCommand("openVerification");
					popup.add(open);
				} else if (learn) {
					open = new JMenuItem("Open Learn View");
					open.addActionListener(this);
					open.addMouseListener(this);
					open.setActionCommand("openLearn");
					popup.add(open);
				}
				if (sim || ver || synth || learn) {
					popup.addSeparator();
				}
				if (sim || ver || learn) {
					JMenuItem copy = new JMenuItem("Copy");
					copy.addActionListener(this);
					copy.addMouseListener(this);
					copy.setActionCommand("copy");
					popup.add(copy);
				}
				if (sim || ver || synth || learn) {
					JMenuItem delete = new JMenuItem("Delete");
					delete.addActionListener(this);
					delete.addMouseListener(this);
					delete.setActionCommand("deleteSim");
					JMenuItem rename = new JMenuItem("Rename");
					rename.addActionListener(this);
					rename.addMouseListener(this);
					rename.setActionCommand("rename");
					popup.add(rename);
					popup.add(delete);
				}
			}
			if (popup.getComponentCount() != 0) {
				popup.show(e.getComponent(), e.getX(), e.getY());
			}
		}
	}

	public void executeMouseClickEvent(MouseEvent e) {
		if (!(e.getSource() instanceof JTree)) {
			enableTabMenu(tab.getSelectedIndex());
			// frame.getGlassPane().setVisible(true);
		} else if (e.getButton() == MouseEvent.BUTTON1 && e.getClickCount() == 2 && e.getSource() instanceof JTree
				&& tree.getFile() != null) {
			if (tree.getFile().length() >= 5 && tree.getFile().substring(tree.getFile().length() - 5).equals(".sbml")
					|| tree.getFile().length() >= 4
					&& tree.getFile().substring(tree.getFile().length() - 4).equals(".xml")) {
				openSBML(tree.getFile(), true);
			} else if (tree.getFile().length() >= 4
					&& tree.getFile().substring(tree.getFile().length() - 4).equals(".gcm")) {
				openModelEditor(false, true);
			} else if (tree.getFile().length() >= 4
					&& tree.getFile().substring(tree.getFile().length() - 4).equals(".vhd")) {
				openModel("VHDL");
			} else if (tree.getFile().length() >= 2
					&& tree.getFile().substring(tree.getFile().length() - 2).equals(".s")) {
				openModel("Assembly File");
			} else if (tree.getFile().length() >= 5
					&& tree.getFile().substring(tree.getFile().length() - 5).equals(".inst")) {
				openModel("Instruction File");
			} else if (tree.getFile().length() >= 5
					&& tree.getFile().substring(tree.getFile().length() - 5).equals(".prop")) { // Dhanashree
				openModel("Property File");
			} else if (tree.getFile().length() >= 5
					&& tree.getFile().substring(tree.getFile().length() - 5).equals(".vams")) {
				openModel("Verilog-AMS");
			} else if (tree.getFile().length() >= 3
					&& tree.getFile().substring(tree.getFile().length() - 3).equals(".sv")) {
				openModel("SystemVerilog");
			} else if (tree.getFile().length() >= 2
					&& tree.getFile().substring(tree.getFile().length() - 2).equals(".g")) {
				openModel("Petri Net");
			} else if (tree.getFile().length() >= 4
					&& tree.getFile().substring(tree.getFile().length() - 4).equals(".lpn")) {
				openLPN();
			} else if (tree.getFile().length() >= 4
					&& tree.getFile().substring(tree.getFile().length() - 4).equals(".csp")) {
				openModel("CSP");
			} else if (tree.getFile().length() >= 4
					&& tree.getFile().substring(tree.getFile().length() - 4).equals(".hse")) {
				openModel("Handshaking Expansion");
			} else if (tree.getFile().length() >= 4
					&& tree.getFile().substring(tree.getFile().length() - 4).equals(".unc")) {
				openModel("Extended Burst-Mode Machine");
			} else if (tree.getFile().length() >= 4
					&& tree.getFile().substring(tree.getFile().length() - 4).equals(".rsg")) {
				openModel("Reduced State Graph");
			} else if (tree.getFile().length() >= 4
					&& tree.getFile().substring(tree.getFile().length() - 4).equals(".cir")) {
				openModel("Spice Circuit");
			} else if (tree.getFile().length() >= 4
					&& tree.getFile().substring(tree.getFile().length() - 4).equals(".grf")) {
				openGraph();
			} else if (tree.getFile().length() >= 4
					&& tree.getFile().substring(tree.getFile().length() - 4).equals(".prb")) {
				openHistogram();
			} else if (new File(tree.getFile()).isDirectory() && !tree.getFile().equals(root)) {
				boolean sim = false;
				boolean synth = false;
				boolean ver = false;
				for (String s : new File(tree.getFile()).list()) {
					if (s.length() > 3 && s.substring(s.length() - 4).equals(".sim")) {
						sim = true;
					} else if ((s.length() > 3 && s.substring(s.length() - 4).equals(".syn"))
							|| s.endsWith(GlobalConstants.SBOL_SYNTH_PROPERTIES_EXTENSION)) {
						synth = true;
					} else if (s.length() > 3 && s.substring(s.length() - 4).equals(".ver")) {
						ver = true;
					} 
				}
				if (sim) {
					try {
						openAnalysisView(tree.getFile(),true);
					} catch (Exception e0) {
						e0.printStackTrace();
					}
				} else if (synth) {
					openSynth();
				} else if (ver) {
					openVerify();
				} 
			} else if (new File(tree.getFile()).isDirectory() && tree.getFile().equals(root)) {
				tree.expandPath(tree.getRoot());
			}
		} else {
			enableTreeMenu();
			return;
		}
		enableTabMenu(tab.getSelectedIndex());
	}

	@Override
	public void mouseMoved(MouseEvent e) {

	}

	@Override
	public void mouseWheelMoved(MouseWheelEvent e) {
		Component glassPane = frame.getGlassPane();
		Point glassPanePoint = e.getPoint();
		// Component component = e.getComponent();
		Container container = frame.getContentPane();
		Point containerPoint = SwingUtilities.convertPoint(glassPane, glassPanePoint, frame.getContentPane());
		if (containerPoint.y < 0) { // we're not in the content pane
			if (containerPoint.y + menuBar.getHeight() >= 0) {
				Component component = menuBar.getComponentAt(glassPanePoint);
				Point componentPoint = SwingUtilities.convertPoint(glassPane, glassPanePoint, component);
				component.dispatchEvent(new MouseWheelEvent(component, e.getID(), e.getWhen(), e.getModifiers(),
						componentPoint.x, componentPoint.y, e.getClickCount(), e.isPopupTrigger(), e.getScrollType(),
						e.getScrollAmount(), e.getWheelRotation()));
				frame.getGlassPane().setVisible(false);
			}
		} else {
			Component deepComponent = SwingUtilities.getDeepestComponentAt(container, containerPoint.x,
					containerPoint.y);
			Point componentPoint = SwingUtilities.convertPoint(glassPane, glassPanePoint, deepComponent);
			// if (deepComponent instanceof ScrollableTabPanel) {
			// deepComponent = tab.findComponentAt(componentPoint);
			// }
			deepComponent.dispatchEvent(new MouseWheelEvent(deepComponent, e.getID(), e.getWhen(), e.getModifiers(),
					componentPoint.x, componentPoint.y, e.getClickCount(), e.isPopupTrigger(), e.getScrollType(),
					e.getScrollAmount(), e.getWheelRotation()));
		}
	}

	private String identifySBOLSynthesisPath(String actionCommand) {
		String[] splitCommand = actionCommand.split("_");
		String synthFilePath = root.replace(new File(root).getName(), splitCommand[0]);
		String synthFileID = "";
		for (int i = 2; i < splitCommand.length; i++) {
			synthFileID = synthFileID + "_" + splitCommand[i];
		}
		synthFilePath = synthFilePath + synthFileID;
		int synthIndex = 1;
		while (new File(synthFilePath + "_" + synthIndex).exists()) {
			synthIndex++;
		}
		synthFilePath = synthFilePath + "_" + synthIndex;
		return synthFilePath;
	}

	private void openSynth() {
		boolean done = false;
		for (int i = 0; i < tab.getTabCount(); i++) {
			if (getTitleAt(i).equals(GlobalConstants.getFilename(tree.getFile()))) {
				tab.setSelectedIndex(i);
				done = true;
			}
		}
		if (!done) {
			JPanel synthPanel = new JPanel();
			// String graphFile = "";
			if (new File(tree.getFile()).isDirectory()) {
				String[] list = new File(tree.getFile()).list();
				int run = 0;
				for (int i = 0; i < list.length; i++) {
					if (!(new File(list[i]).isDirectory()) && list[i].length() > 4) {
						String end = "";
						for (int j = 1; j < 5; j++) {
							end = list[i].charAt(list[i].length() - j) + end;
						}
						if (end.equals(".tsd") || end.equals(".dat") || end.equals(".csv")) {
							if (list[i].contains("run-")) {
								int tempNum = Integer
										.parseInt(list[i].substring(4, list[i].length() - end.length()));
								if (tempNum > run) {
									run = tempNum;
								}
							}
						}
					}
				}
			}

			String synthFile = tree.getFile() + File.separator
					+ GlobalConstants.getFilename(tree.getFile()) + ".syn";
			String synthFile2 = tree.getFile() + File.separator + ".syn";
			Properties load = new Properties();
			String synthesisFile = "";
			try {
				if (new File(synthFile2).exists()) {
					FileInputStream in = new FileInputStream(new File(synthFile2));
					load.load(in);
					in.close();
					new File(synthFile2).delete();
				}
				if (new File(synthFile).exists()) {
					FileInputStream in = new FileInputStream(new File(synthFile));
					load.load(in);
					in.close();
					if (load.containsKey("synthesis.file")) {
						synthesisFile = load.getProperty("synthesis.file");
						synthesisFile = GlobalConstants.getFilename(synthesisFile);
					}
				}
				// FileOutputStream out = new FileOutputStream(new
				// File(synthesisFile));
				// load.store(out, synthesisFile);
				// out.close();

			} catch (Exception e) {
				JOptionPane.showMessageDialog(frame, "Unable to load properties file!", "Error Loading Properties",
						JOptionPane.ERROR_MESSAGE);
			}
			for (int i = 0; i < tab.getTabCount(); i++) {
				if (getTitleAt(i).equals(synthesisFile)) {
					tab.setSelectedIndex(i);
					if (save(i, 0) == 0) {
						return;
					}
					break;
				}
			}
			if (!(new File(root + File.separator + synthesisFile).exists())) {
				JOptionPane.showMessageDialog(frame,
						"Unable to open view because " + synthesisFile + " is missing.", "Error",
						JOptionPane.ERROR_MESSAGE);
				return;
			}
			// if (!graphFile.equals("")) {
			SynthesisViewATACS synth = new SynthesisViewATACS(tree.getFile(), "flag", log, this);
			// synth.addMouseListener(this);
			synthPanel.add(synth);
			addTab(GlobalConstants.getFilename(tree.getFile()),	synthPanel, "Synthesis");
		}
	}

	private void openVerify() {
		boolean done = false;
		for (int i = 0; i < tab.getTabCount(); i++) {
			if (getTitleAt(i).equals(GlobalConstants.getFilename(tree.getFile()))) {
				tab.setSelectedIndex(i);
				done = true;
			}
		}
		if (!done) {
			// JPanel verPanel = new JPanel();
			// JPanel abstPanel = new JPanel();
			// JPanel verTab = new JTabbedPane();
			// String graphFile = "";
			/*
			 * if (new File(tree.getFile()).isDirectory()) { String[] list = new
			 * File(tree.getFile()).list(); int run = 0; for (int i = 0; i <
			 * list.length; i++) { if (!(new File(list[i]).isDirectory()) &&
			 * list[i].length() > 4) { String end = ""; for (int j = 1; j < 5;
			 * j++) { end = list[i].charAt(list[i].length() - j) + end; } if
			 * (end.equals(".tsd") || end.equals(".dat") || end.equals(".csv"))
			 * { if (list[i].contains("run-")) { int tempNum =
			 * Integer.parseInt(list[i].substring(4, list[i] .length() -
			 * end.length())); if (tempNum > run) { run = tempNum; // graphFile
			 * = tree.getFile() + separator + // list[i]; } } } } } }
			 */

			String verName = GlobalConstants.getFilename(tree.getFile());
			String verFile = tree.getFile() + File.separator + verName + ".ver";
			Properties load = new Properties();
			String verifyFile = "";
			try {
				if (new File(verFile).exists()) {
					FileInputStream in = new FileInputStream(new File(verFile));
					load.load(in);
					in.close();
					if (load.containsKey("verification.file")) {
						verifyFile = load.getProperty("verification.file");
						verifyFile = GlobalConstants.getFilename(verifyFile);
					}
				}
				// FileOutputStream out = new FileOutputStream(new
				// File(verifyFile));
				// load.store(out, verifyFile);
				// out.close();
			} catch (Exception e) {
				JOptionPane.showMessageDialog(frame, "Unable to load properties file!", "Error Loading Properties",
						JOptionPane.ERROR_MESSAGE);
			}
			for (int i = 0; i < tab.getTabCount(); i++) {
				if (getTitleAt(i).equals(verifyFile)) {
					tab.setSelectedIndex(i);
					if (save(i, 0) == 0) {
						return;
					}
					break;
				}
			}
			if (!(new File(verFile).exists())) {
				JOptionPane.showMessageDialog(frame, "Unable to open view because " + verifyFile + " is missing.",
						"Error", JOptionPane.ERROR_MESSAGE);
				return;
			}
			// if (!graphFile.equals("")) {
			VerificationView ver = new VerificationView(root + File.separator + verName, verName, "flag", log, this,
					false, true);
			// ver.addMouseListener(this);
			// verPanel.add(ver);
			// AbstPane abst = new AbstPane(root + separator + verName, ver,
			// "flag", log, this, lema,
			// atacs);
			// abstPanel.add(abst);
			// verTab.add("verify", verPanel);
			// verTab.add("abstract", abstPanel);
			addTab(GlobalConstants.getFilename(tree.getFile()), ver, "Verification");
		}
	}

	/**
	 * This is the main method. It excecutes the BioSim GUI FrontEnd program.
	 */
	public static void main(String args[]) {
		String message = "";

		if (System.getProperty("os.name").toLowerCase().startsWith("mac os")) {
			if (System.getenv("DDLD_LIBRARY_PATH") == null) {
				System.out.println("DDLD_LIBRARY_PATH is missing");
			}
		}

		Executables.checkExecutables();
		ArrayList<String> errors = Executables.getErrors();
		if (errors.size()>0) {
			message = "<html>WARNING: Some external components are missing or have problems.<br>" +
					"You may continue but with some loss of functionality.";	
			System.err.println("WARNING: Some external components are missing or have problems.\n" +
					"You may continue but with some loss of functionality.");	
			for (int i = 0; i < errors.size(); i++) {
				message += "<br>" + errors.get(i);
				System.err.println(errors.get(i));
			}
			message += "<br></html>";

			JPanel msgPanel = new JPanel(new BorderLayout());
			JPanel checkPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT));
			JLabel msg = new JLabel(message);
			msgPanel.add(msg, BorderLayout.NORTH);
			JCheckBox jcb = new JCheckBox("Do not ask me again");
			jcb.addActionListener(new ActionListener() {

				@Override
				public void actionPerformed(ActionEvent arg0) {
					// if-else statement below is used to update checkbox in settings whenever this one is changed.
					if (jcb.isSelected()) {
						Preferences.userRoot().put("atacs.ignore.external.warnings", "true");
					} 

				}
			});
			checkPanel.add(jcb);
			msgPanel.add(checkPanel, BorderLayout.SOUTH);
			if (!Preferences.userRoot().get("atacs.ignore.external.warnings", "").equals("true")) {
				int value = JOptionPane.showConfirmDialog(null , msgPanel , "Problems with External Components" , JOptionPane.OK_CANCEL_OPTION);
				if (value == JOptionPane.CANCEL_OPTION) return;
			}
		} else {
			Preferences.userRoot().put("atacs.ignore.external.warnings", "false");
		}
		new atacsGui();
	}

	public void updateAsyncViews(String updatedFile) {
		for (int i = 0; i < tab.getTabCount(); i++) {
			String tab = this.getTitleAt(i);
			String properties = root + File.separator + tab + File.separator + tab + ".ver";
			String properties1 = root + File.separator + tab + File.separator + tab + ".synth";
			String properties2 = root + File.separator + tab + File.separator + tab + ".lrn";
			if (new File(properties).exists()) {
				VerificationView verify = ((VerificationView) (this.tab.getComponentAt(i)));
				verify.reload();
			}
			if (new File(properties1).exists()) {
				JTabbedPane sim = ((JTabbedPane) (this.tab.getComponentAt(i)));
				for (int j = 0; j < sim.getTabCount(); j++) {
					if (sim.getComponentAt(j).getName().equals("Synthesis")) {
						((SynthesisViewATACS) (sim.getComponentAt(j))).reload(updatedFile);
					}
				}
			}
			// }
			if (new File(properties2).exists()) {
				String check = "";
				try {
					Properties p = new Properties();
					FileInputStream load = new FileInputStream(new File(properties2));
					p.load(load);
					load.close();
					if (p.containsKey("learn.file")) {
						String[] getProp = GlobalConstants.splitPath(p.getProperty("learn.file"));
						check = getProp[getProp.length - 1];
					} else {
						check = "";
					}
				} catch (Exception e) {
					JOptionPane.showMessageDialog(frame, "Unable to load background file.", "Error",
							JOptionPane.ERROR_MESSAGE);
					check = "";
				}
				if (check.equals(updatedFile)) {
					JTabbedPane learn = ((JTabbedPane) (this.tab.getComponentAt(i)));
					for (int j = 0; j < learn.getTabCount(); j++) {
						if (learn.getComponentAt(j).getName().equals("Data Manager")) {
							((DataManager) (learn.getComponentAt(j))).updateSpecies();
						} else if (learn.getComponentAt(j).getName().equals("Learn Options")) {
							// TODO: removed
						} else if (learn.getComponentAt(j).getName().contains("Graph")) {
							((Graph) (learn.getComponentAt(j))).refresh();
						}
					}
				}
			}
		}
	}

	public void enableTabMenu(int selectedTab) {
		saveButton.setEnabled(false);
		saveasButton.setEnabled(false);
		runButton.setEnabled(false);
		refreshButton.setEnabled(false);
		checkButton.setEnabled(false);
		exportButton.setEnabled(false);
		save.setEnabled(false);
		saveAs.setEnabled(false);
		// saveSBOL.setEnabled(false);
		saveModel.setEnabled(false);
		saveAll.setEnabled(false);
		close.setEnabled(false);
		closeAll.setEnabled(false);
		run.setEnabled(false);
		check.setEnabled(false);
		exportMenu.setEnabled(true);
		exportArchive.setEnabled(true);
		exportSBML.setEnabled(false);
		exportFlatSBML.setEnabled(false);
		exportDataMenu.setEnabled(false);
		exportImageMenu.setEnabled(false);
		exportMovieMenu.setEnabled(false);
		exportCsv.setEnabled(false);
		exportDat.setEnabled(false);
		exportEps.setEnabled(false);
		exportJpg.setEnabled(false);
		exportPdf.setEnabled(false);
		exportPng.setEnabled(false);
		exportSvg.setEnabled(false);
		exportTsd.setEnabled(false);
		exportAvi.setEnabled(false);
		exportMp4.setEnabled(false);
		saveAsVerilog.setEnabled(false);
		viewCircuit.setEnabled(false);
		viewLog.setEnabled(false);
		refresh.setEnabled(false);
		select.setEnabled(false);
		cut.setEnabled(false);
		addModule.setEnabled(false);
		addVariable.setEnabled(false);
		addBoolean.setEnabled(false);
		addPlace.setEnabled(false);
		addTransition.setEnabled(false);
		addRule.setEnabled(false);
		addConstraint.setEnabled(false);
		moveLeft.setEnabled(false);
		moveRight.setEnabled(false);
		moveUp.setEnabled(false);
		moveDown.setEnabled(false);
		undo.setEnabled(false);
		redo.setEnabled(false);
		if (selectedTab != -1) {
			tab.setSelectedIndex(selectedTab);
		}
		Component comp = tab.getSelectedComponent();
		if (comp instanceof ModelEditor) {
			saveButton.setEnabled(true);
			saveasButton.setEnabled(true);
			checkButton.setEnabled(true);
			exportButton.setEnabled(true);
			save.setEnabled(true);
			saveAs.setEnabled(true);
			// saveSBOL.setEnabled(true);
			saveAll.setEnabled(true);
			close.setEnabled(true);
			closeAll.setEnabled(true);
			check.setEnabled(true);
			select.setEnabled(true);
			cut.setEnabled(true);
			if (!((ModelEditor) comp).isGridEditor()) {
				addVariable.setEnabled(true);
				addBoolean.setEnabled(true);
				addPlace.setEnabled(true);
				addTransition.setEnabled(true);
				addRule.setEnabled(true);
				addConstraint.setEnabled(true);
				moveLeft.setEnabled(true);
				moveRight.setEnabled(true);
				moveUp.setEnabled(true);
				moveDown.setEnabled(true);
			}
			addModule.setEnabled(true);
			undo.setEnabled(true);
			redo.setEnabled(true);
			exportMenu.setEnabled(true);
			exportSBML.setEnabled(true);
			exportFlatSBML.setEnabled(true);
			exportImageMenu.setEnabled(true);
			exportJpg.setEnabled(true);
		} else if (comp instanceof LHPNEditor) {
			saveButton.setEnabled(true);
			saveasButton.setEnabled(true);
			save.setEnabled(true);
			saveAs.setEnabled(true);
			saveAll.setEnabled(true);
			close.setEnabled(true);
			closeAll.setEnabled(true);
			viewCircuit.setEnabled(true);
			exportMenu.setEnabled(true);
			exportSBML.setEnabled(true);
			exportFlatSBML.setEnabled(true);
		} else if (comp instanceof Graph) {
			saveButton.setEnabled(true);
			saveasButton.setEnabled(true);
			refreshButton.setEnabled(true);
			exportButton.setEnabled(true);
			save.setEnabled(true);
			saveAs.setEnabled(true);
			saveAll.setEnabled(true);
			close.setEnabled(true);
			closeAll.setEnabled(true);
			refresh.setEnabled(true);
			exportMenu.setEnabled(true);
			exportImageMenu.setEnabled(true);
			if (((Graph) comp).isTSDGraph()) {
				exportDataMenu.setEnabled(true);
				exportCsv.setEnabled(true);
				exportDat.setEnabled(true);
				exportTsd.setEnabled(true);
			}
			exportEps.setEnabled(true);
			exportJpg.setEnabled(true);
			exportPdf.setEnabled(true);
			exportPng.setEnabled(true);
			exportSvg.setEnabled(true);
		} else if (comp instanceof JTabbedPane) {
			Component component = ((JTabbedPane) comp).getSelectedComponent();
			Component learnComponent = null;
			Boolean learn = false;
			Boolean learnLHPN = false;
			for (Component c : ((JTabbedPane) comp).getComponents()) {
				if (c instanceof LearnView) {
					learn = true;
					learnComponent = c;
				}
			}
			if (component instanceof Graph) {
				saveButton.setEnabled(true);
				saveasButton.setEnabled(true);
				runButton.setEnabled(true);
				refreshButton.setEnabled(true);
				exportButton.setEnabled(true);
				save.setEnabled(true);
				saveAll.setEnabled(true);
				close.setEnabled(true);
				closeAll.setEnabled(true);
				run.setEnabled(true);
				if (learn && learnComponent != null) {
					run.setEnabled(true);
					saveModel.setEnabled(((LearnView) learnComponent).getViewModelEnabled());
					saveAsVerilog.setEnabled(false);
					viewCircuit.setEnabled(((LearnView) learnComponent).getViewModelEnabled());
					viewLog.setEnabled(((LearnView) learnComponent).getViewLogEnabled());
				} else if (learnLHPN && learnComponent != null) {
					// TODO: removed
				}
				saveAs.setEnabled(true);
				refresh.setEnabled(true);
				exportMenu.setEnabled(true);
				exportImageMenu.setEnabled(true);
				if (((Graph) component).isTSDGraph()) {
					exportDataMenu.setEnabled(true);
					exportCsv.setEnabled(true);
					exportDat.setEnabled(true);
					exportTsd.setEnabled(true);
				}
				exportEps.setEnabled(true);
				exportJpg.setEnabled(true);
				exportPdf.setEnabled(true);
				exportPng.setEnabled(true);
				exportSvg.setEnabled(true);
			} else if (component instanceof AnalysisView) {
				saveButton.setEnabled(true);
				runButton.setEnabled(true);
				save.setEnabled(true);
				saveAll.setEnabled(true);
				close.setEnabled(true);
				closeAll.setEnabled(true);
				run.setEnabled(true);
			} else if (component instanceof MovieContainer) {
				exportMenu.setEnabled(true);
				exportMovieMenu.setEnabled(true);
				exportAvi.setEnabled(true);
				exportMp4.setEnabled(true);
				exportImageMenu.setEnabled(true);
				exportJpg.setEnabled(true);
				saveButton.setEnabled(true);
				runButton.setEnabled(true);
				save.setEnabled(true);
				saveAll.setEnabled(true);
				close.setEnabled(true);
				closeAll.setEnabled(true);
				run.setEnabled(true);
			} else if (component instanceof ModelEditor) {
				saveButton.setEnabled(true);
				runButton.setEnabled(true);
				save.setEnabled(true);
				saveAll.setEnabled(true);
				close.setEnabled(true);
				closeAll.setEnabled(true);
				run.setEnabled(true);
			} else if (component instanceof LearnView) {
				saveButton.setEnabled(true);
				runButton.setEnabled(true);
				save.setEnabled(true);
				saveAll.setEnabled(true);
				close.setEnabled(true);
				closeAll.setEnabled(true);
				run.setEnabled(true);
				viewCircuit.setEnabled(((LearnView) component).getViewModelEnabled());
				viewLog.setEnabled(((LearnView) component).getViewLogEnabled());
				saveModel.setEnabled(((LearnView) component).getViewModelEnabled());
			} else if (component instanceof DataManager) {
				saveButton.setEnabled(true);
				runButton.setEnabled(true);
				save.setEnabled(true);
				saveAs.setEnabled(true);
				saveAll.setEnabled(true);
				close.setEnabled(true);
				closeAll.setEnabled(true);
				if (learn && learnComponent != null) {
					run.setEnabled(true);
					saveModel.setEnabled(((LearnView) learnComponent).getViewModelEnabled());
					viewCircuit.setEnabled(((LearnView) learnComponent).getViewModelEnabled());
					viewLog.setEnabled(((LearnView) learnComponent).getViewLogEnabled());
				} 
			} else if (component instanceof JPanel) {
				saveButton.setEnabled(true);
				runButton.setEnabled(true);
				save.setEnabled(true);
				saveAll.setEnabled(true);
				close.setEnabled(true);
				closeAll.setEnabled(true);
				run.setEnabled(true);
				if (learn && learnComponent != null) {
					run.setEnabled(true);
					saveModel.setEnabled(((LearnView) learnComponent).getViewModelEnabled());
					viewCircuit.setEnabled(((LearnView) learnComponent).getViewModelEnabled());
					viewLog.setEnabled(((LearnView) learnComponent).getViewLogEnabled());
				} 
			} else if (component instanceof JScrollPane) {
				saveButton.setEnabled(true);
				runButton.setEnabled(true);
				save.setEnabled(true);
				saveAll.setEnabled(true);
				close.setEnabled(true);
				closeAll.setEnabled(true);
				run.setEnabled(true);
			}
		} else if (comp instanceof JPanel) {
			if (comp.getName().equals("Verification")) {
				saveButton.setEnabled(true);
				saveasButton.setEnabled(true);
				runButton.setEnabled(true);
				save.setEnabled(true);
				saveAll.setEnabled(true);
				close.setEnabled(true);
				closeAll.setEnabled(true);
				run.setEnabled(true);
				viewTrace.setEnabled(((VerificationView) comp).getViewTraceEnabled());
				viewLog.setEnabled(((VerificationView) comp).getViewLogEnabled());
			} else if (comp.getName().equals("Synthesis")) {
				saveButton.setEnabled(true);
				saveasButton.setEnabled(true);
				runButton.setEnabled(true);
				save.setEnabled(true);
				saveAll.setEnabled(true);
				close.setEnabled(true);
				closeAll.setEnabled(true);
				run.setEnabled(true);
				viewRules.setEnabled(
						true/*
						 * ((Synthesis) comp).getViewRulesEnabled()
						 */);
				viewTrace.setEnabled(
						true/*
						 * ((Synthesis) comp).getViewTraceEnabled()
						 */);
				viewCircuit.setEnabled(
						true/*
						 * ((Synthesis) comp).getViewCircuitEnabled()
						 */);
				viewLog.setEnabled(
						true/*
						 * ((Synthesis) comp).getViewLogEnabled()
						 */);
			}
		} else if (comp instanceof JScrollPane) {
			saveButton.setEnabled(true);
			saveasButton.setEnabled(true);
			save.setEnabled(true);
			saveAll.setEnabled(true);
			close.setEnabled(true);
			closeAll.setEnabled(true);
			saveAs.setEnabled(true);
		}
	}

	private void enableTreeMenu() {
		viewModGraph.setEnabled(false);
		createAnal.setEnabled(false);
		createSynth.setEnabled(false);
		createLearn.setEnabled(false);
		createVer.setEnabled(false);
		createSbml.setEnabled(false);
		check.setEnabled(false);
		copy.setEnabled(false);
		rename.setEnabled(false);
		delete.setEnabled(false);
		viewModel.setEnabled(false);
		viewRules.setEnabled(false);
		viewTrace.setEnabled(false);
		viewCircuit.setEnabled(false);
		saveAsVerilog.setEnabled(false);
		if (tree.getFile() != null) {
			if (tree.getFile().equals(root)) {
			} else if (tree.getFile().endsWith(".sbol")) {
			} else if (tree.getFile().endsWith(".sbml") || tree.getFile().endsWith(".xml")) {
				viewModGraph.setEnabled(true);
				viewModGraph.setActionCommand("graph");
				createAnal.setEnabled(true);
				createAnal.setActionCommand("createAnalysis");
				createSynth.setEnabled(true);
				createSynth.setActionCommand("createSynthesis");
				createLearn.setEnabled(true);
				copy.setEnabled(true);
				rename.setEnabled(true);
				delete.setEnabled(true);
				viewModel.setEnabled(true);
			} else if (tree.getFile().endsWith(".grf")) {
				copy.setEnabled(true);
				rename.setEnabled(true);
				delete.setEnabled(true);
			} else if (tree.getFile().endsWith(".vams")) {
				copy.setEnabled(true);
				rename.setEnabled(true);
				delete.setEnabled(true);
			} else if (tree.getFile().endsWith(".sv")) {
				copy.setEnabled(true);
				rename.setEnabled(true);
				delete.setEnabled(true);
			} else if (tree.getFile().endsWith(".g")) {
				viewModel.setEnabled(true);
				viewModGraph.setEnabled(true);
				createSynth.setEnabled(true);
				createSynth.setActionCommand("createSynthesis");
				createVer.setEnabled(true);
				copy.setEnabled(true);
				rename.setEnabled(true);
				delete.setEnabled(true);
			} else if (tree.getFile().endsWith(".lpn")) {
				viewModel.setEnabled(true);
				viewModGraph.setEnabled(true);
				createAnal.setEnabled(true);
				createAnal.setActionCommand("createAnalysis");
				createSynth.setEnabled(true);
				createSynth.setActionCommand("createSynthesis");
				createVer.setEnabled(true);
				copy.setEnabled(true);
				rename.setEnabled(true);
				delete.setEnabled(true);
				saveAsVerilog.setEnabled(true);
			} else if (tree.getFile().endsWith(".hse") || tree.getFile().endsWith(".unc")
					|| tree.getFile().endsWith(".csp") || tree.getFile().endsWith(".vhd")
					|| tree.getFile().endsWith(".rsg")) {
				viewModel.setEnabled(true);
				viewModGraph.setEnabled(true);
				createSynth.setEnabled(true);
				createSynth.setActionCommand("createSynthesis");
				createVer.setEnabled(true);
				copy.setEnabled(true);
				rename.setEnabled(true);
				delete.setEnabled(true);
			} else if (tree.getFile().endsWith(".s") || tree.getFile().endsWith(".inst")) {
				createAnal.setEnabled(true);
				createVer.setEnabled(true);
				copy.setEnabled(true);
				rename.setEnabled(true);
				delete.setEnabled(true);
			} else if (new File(tree.getFile()).isDirectory()) {
				boolean sim = false;
				boolean synth = false;
				boolean ver = false;
				boolean learn = false;
				for (String s : new File(tree.getFile()).list()) {
					if (s.endsWith(".sim")) {
						sim = true;
					} else if ((s.endsWith(".syn")) || (s.endsWith(GlobalConstants.SBOL_SYNTH_PROPERTIES_EXTENSION))) {
						synth = true;
					} else if (s.endsWith(".ver")) {
						ver = true;
					} else if (s.endsWith(".lrn")) {
						learn = true;
					}
				}
				if (sim || synth || ver || learn) {
					copy.setEnabled(true);
					rename.setEnabled(true);
					delete.setEnabled(true);
				}
			}
		}
	}

	@Override
	public void update(Observable o, Object arg) {
		// TODO Auto-generated method stub
		
	}
}
