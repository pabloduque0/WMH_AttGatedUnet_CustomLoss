""" This module loads all the classes from the wrapped CTK libraries into
its namespace."""

__kits_to_load = [ 'QtTesting', 'Testing', 'VisualizationVTKWidgets', 'VisualizationVTKCore', 'ScriptingPythonWidgets', 'ScriptingPythonCore', 'ImageProcessingITKCore', 'DICOMWidgets', 'DICOMCore', 'Widgets', 'Core',  ]

# Set to True when debugging
_CTK_VERBOSE_IMPORT = False

for kit in __kits_to_load:
  try:
    exec "from CTK%sPythonQt import *" % (kit)
  except ImportError as detail:
    if _CTK_VERBOSE_IMPORT:
      print detail

def add_methodclass_to_ctkWorkflowStep_or_ctkWorkflowWidgetStep(workflowstep_class):

  def validate(self, validationSucceed, desiredBranchId):
    """Validates the computation performed in this step's processing state."""
    self.ctkWorkflowStepQObject().validationComplete(validationSucceed, desiredBranchId)

  def onEntry(self, comingFrom, transitionType):
    """Reimplement this function for step-specific processing when entering a step."""
    self.ctkWorkflowStepQObject().onEntryComplete()

  def onExit(self, comingFrom, transitionType):
    """Reimplement this function for step-specific processing when exiting a step."""
    self.ctkWorkflowStepQObject().onExitComplete()

  def initialize(self, stepid):
    workflowstep_class.__init__(self)
    self.setId(stepid)
    self.setHasValidateCommand(True)
    self.setHasOnEntryCommand(True)
    self.setHasOnExitCommand(True)

    qobj = self.ctkWorkflowStepQObject()

    qobj.connect('invokeValidateCommand(const QString&)', self.validate)

    qobj.connect('invokeOnEntryCommand(const ctkWorkflowStep*, \
      ctkWorkflowInterstepTransition::InterstepTransitionType)', self.onEntry)

    qobj.connect('invokeOnExitCommand(const ctkWorkflowStep*, \
      ctkWorkflowInterstepTransition::InterstepTransitionType)', self.onExit)

  workflowstep_class.validate = validate
  workflowstep_class.onEntry = onEntry
  workflowstep_class.onExit = onExit
  workflowstep_class.initialize = initialize

def add_methodclass_to_ctkWorkflowWidgetStep():

  def createUserInterface(self):
    self.ctkWorkflowStepQObject().createUserInterfaceComplete()

  #def showUserInterface(self):
  #  self.ctkWorkflowStepQObject().showUserInterfaceComplete()

  ctkWorkflowWidgetStep.createUserInterface = createUserInterface;
  #ctkWorkflowWidgetStep.showUserInterface = showUserInterface;

def decorates_ctkWorkflowWidgetStep_initialize_method():
  """Decorates ctkWorkflowWidgetStep::initialize() method.
  The properties 'hasCreateUserInterfaceCommand' and 'hasShowUserInterfaceCommand'
  are set to True.
  Signals 'invokeCreateUserInterfaceCommand' and 'invokeShowUserInterfaceCommand'
  are respectively connected to the slots 'createUserInterface' and 'showUserInterface'.
  """

  f = ctkWorkflowWidgetStep.initialize

  from functools import wraps
  @wraps(f)
  def decorated(self, *args, **kwargs):
    f(self, *args, **kwargs)
    self.setHasCreateUserInterfaceCommand(True)
    #self.setHasShowUserInterfaceCommand(True)

    qobj = self.ctkWorkflowStepQObject()

    qobj.connect('invokeCreateUserInterfaceCommand()', self.createUserInterface)
    #qobj.connect('invokeShowUserInterfaceCommand()', self.showUserInterface)

  ctkWorkflowWidgetStep.initialize = decorated

#
# Decorators
#

_lib = next((_lib for _lib in __kits_to_load if _lib == 'Core'), None)
if _lib == 'Core':
  add_methodclass_to_ctkWorkflowStep_or_ctkWorkflowWidgetStep(ctkWorkflowStep)

_lib = next((_lib for _lib in __kits_to_load if _lib == 'Widgets'), None)
if _lib == 'Widgets':
  add_methodclass_to_ctkWorkflowStep_or_ctkWorkflowWidgetStep(ctkWorkflowWidgetStep)
  add_methodclass_to_ctkWorkflowWidgetStep()
  decorates_ctkWorkflowWidgetStep_initialize_method()

# Removing things the user shouldn't have to see.
del __kits_to_load, _lib, _CTK_VERBOSE_IMPORT
del add_methodclass_to_ctkWorkflowStep_or_ctkWorkflowWidgetStep
del add_methodclass_to_ctkWorkflowWidgetStep, decorates_ctkWorkflowWidgetStep_initialize_method
