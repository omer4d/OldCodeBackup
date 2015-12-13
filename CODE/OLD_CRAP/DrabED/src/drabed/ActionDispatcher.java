package drabed;

import java.util.*;
import java.awt.event.*;

public class ActionDispatcher {
    private LinkedList<ActionListener> mActionListenerList;
    
    public ActionDispatcher()
    {
        mActionListenerList = new LinkedList<ActionListener>();
    }

    void addActionListener(ActionListener al)
    {
        if (al != null && !mActionListenerList.contains(al))
            mActionListenerList.add(al);
    }

    void removeActionListener(ActionListener al)
    {
        if (al == null)
            return;
        mActionListenerList.remove(al);
    }

    private void fireActionPerformed(ActionEvent e)
    {
        Iterator<ActionListener> itr = mActionListenerList.iterator();

        while (itr.hasNext())
            itr.next().actionPerformed(e);
    }
}
