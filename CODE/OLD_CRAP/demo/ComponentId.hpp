/* 
 * File:   ComponentId.hpp
 * Author: Stas
 *
 * Created on February 4, 2012, 9:59 PM
 */

#ifndef COMPONENTID_HPP
#define	COMPONENTID_HPP

// ***Values MUST be sequential!!!

enum ComponentId {
    CONTAINER_COMP, OPENABLE_CONTAINER_COMP, IDENTITY_COMP, TAKABLE_COMP, SOUND_COMP,
    RENDERER_COMP, TRACK_FOLLOWER_COMP, NAVIGATOR_COMP,
    
    
    COMPONENT_TYPE_NUM // *** MUST ALWAYS BE THE LAST!
};

#endif	/* COMPONENTID_HPP */