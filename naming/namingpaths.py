import os
import sys

NAMING = os.path.dirname(os.path.abspath(__file__)).replace("\\", "/")
sys.path.insert(0, os.path.dirname(NAMING))

import kitpaths

GAME = kitpaths.REPO
LABEL = os.environ.get("DQIX_LABEL_REPO", os.path.join(os.path.dirname(kitpaths.KIT), "dqix-label")).replace("\\", "/")
