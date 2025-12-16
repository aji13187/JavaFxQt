package com.example.javafxqt;

import com.sun.jna.Library;
import com.sun.jna.Native;
import com.sun.jna.Pointer;

public interface CocoaLibrary extends Library {
    CocoaLibrary INSTANCE = Native.load("objc", CocoaLibrary.class);

    Pointer objc_getClass(String className);
    Pointer sel_registerName(String selectorName);
    Pointer objc_msgSend(Pointer receiver, Pointer selector, Object... args);
}