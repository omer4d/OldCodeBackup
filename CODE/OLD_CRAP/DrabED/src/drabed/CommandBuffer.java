/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package drabed;

import java.util.*;

/**
 *
 * @author Stas
 */

class CommandBufferListener {
    void onAdd(CommandBuffer buff) {}
    void onRedo(CommandBuffer buff) {}
    void onUndo(CommandBuffer buff) {}
    void onClear(CommandBuffer buff) {}
}

public class CommandBuffer {
    private LinkedList<Command> mHistory, mTemp;
    private CommandBufferListener mListener;
    
    CommandBuffer()
    {
        mHistory = new LinkedList<Command>();
        mTemp = new LinkedList<Command>();
    }
    
    void setListener(CommandBufferListener listener)
    {
        mListener = listener;
    }
    
    void add(Command cmd)
    {
        mTemp.clear();
        mHistory.push(cmd);
        
        if(mListener != null) mListener.onAdd(this);
    }
    
    void execute()
    {
        Iterator<Command> iter = mHistory.descendingIterator();
        
        while(iter.hasNext())
            iter.next().execute();
    }
    
    boolean undo()
    {
        if(mHistory.isEmpty()) return false;
        
        Command cmd = mHistory.pop();
        cmd.unexecute();
        mTemp.push(cmd);
        
        if(mListener != null) mListener.onUndo(this);
        return true;
    }
    
    boolean redo()
    {
        if(mTemp.isEmpty()) return false;

        Command cmd = mTemp.pop();
        cmd.execute();
        mHistory.push(cmd);
        
        if(mListener != null) mListener.onRedo(this);
        return true;
    }
    
    void clear()
    {
        mHistory.clear();
        mTemp.clear();
        if(mListener != null) mListener.onClear(this);
    }
    
    boolean canUndo() { return !mHistory.isEmpty(); }
    boolean canRedo() { return !mTemp.isEmpty(); }
}
