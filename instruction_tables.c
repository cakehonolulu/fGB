#include <instruction_tables.h>

const struct instructions instrs[256] = {
    /*          +0	      +1	       +2	        +3	      +4	    +5	      +6	    +7	      +8	    +9	      +A	    +B	         +C	       +D	     +E	       +F      */
    /* 00+ */ { NULL }, { NULL    }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL    },
    /* 10+ */ { NULL }, { NULL    }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL    },
    /* 20+ */ { NULL }, { instr21 }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL    },
    /* 30+ */ { NULL }, { instr31 }, { instr32 }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL    },
    /* 40+ */ { NULL }, { NULL    }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL    },
    /* 50+ */ { NULL }, { NULL    }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL    },
    /* 60+ */ { NULL }, { NULL    }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL    },
    /* 70+ */ { NULL }, { NULL    }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL    },
    /* 80+ */ { NULL }, { NULL    }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL    },
    /* 90+ */ { NULL }, { NULL    }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL    },
    /* A0+ */ { NULL }, { NULL    }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { instrAF },
    /* B0+ */ { NULL }, { NULL    }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL    },
    /* C0+ */ { NULL }, { NULL    }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { instrCB }, { NULL }, { NULL }, { NULL }, { NULL    },
    /* D0+ */ { NULL }, { NULL    }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL    },
    /* E0+ */ { NULL }, { NULL    }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL    },
    /* F0+ */ { NULL }, { NULL    }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL    }, { NULL }, { NULL }, { NULL }, { NULL    }  
};

const struct extended_instructions extended_instrs[256] = {
    /*          +0	      +1	       +2	     +3	       +4	     +5	       +6	     +7	       +8	     +9	       +A	     +B	       +C	     +D	       +E	     +F      */
    /* 00+ */ { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL      }, { NULL }, { NULL }, { NULL }, /* 00+ */
    /* 10+ */ { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL      }, { NULL }, { NULL }, { NULL }, /* 10+ */
    /* 20+ */ { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL      }, { NULL }, { NULL }, { NULL }, /* 20+ */
    /* 30+ */ { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL      }, { NULL }, { NULL }, { NULL }, /* 30+ */
    /* 40+ */ { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL      }, { NULL }, { NULL }, { NULL }, /* 40+ */
    /* 50+ */ { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL      }, { NULL }, { NULL }, { NULL }, /* 50+ */
    /* 60+ */ { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL      }, { NULL }, { NULL }, { NULL }, /* 60+ */
    /* 70+ */ { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { exinstr7C }, { NULL }, { NULL }, { NULL }, /* 70+ */
    /* 80+ */ { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL      }, { NULL }, { NULL }, { NULL }, /* 80+ */
    /* 90+ */ { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL      }, { NULL }, { NULL }, { NULL }, /* 90+ */
    /* A0+ */ { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL      }, { NULL }, { NULL }, { NULL }, /* A0+ */
    /* B0+ */ { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL      }, { NULL }, { NULL }, { NULL }, /* B0+ */
    /* C0+ */ { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL      }, { NULL }, { NULL }, { NULL }, /* C0+ */
    /* D0+ */ { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL      }, { NULL }, { NULL }, { NULL }, /* D0+ */
    /* E0+ */ { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL      }, { NULL }, { NULL }, { NULL }, /* E0+ */
    /* F0+ */ { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL }, { NULL      }, { NULL }, { NULL }, { NULL }  /* F0+ */ 
};

