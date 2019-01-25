/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    SUMOSAXAttributesImpl_Xerces.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Encapsulated Xerces-SAX-attributes
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <cassert>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <utils/common/RGBColor.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/common/StringUtils.h>
#include <utils/geom/Boundary.h>
#include <utils/geom/PositionVector.h>
#include "XMLSubSys.h"
#include "SUMOSAXAttributesImpl_Xerces.h"
#include "SUMOSAXAttributesImpl_Cached.h"


// ===========================================================================
// class definitions
// ===========================================================================
SUMOSAXAttributesImpl_Xerces::SUMOSAXAttributesImpl_Xerces(const XERCES_CPP_NAMESPACE::Attributes& attrs,
        const std::map<int, XMLCh*>& predefinedTags,
        const std::map<int, std::string>& predefinedTagsMML,
        const std::string& objectType) :
    SUMOSAXAttributes(objectType),
    myAttrs(attrs),
    myPredefinedTags(predefinedTags),
    myPredefinedTagsMML(predefinedTagsMML) { }


SUMOSAXAttributesImpl_Xerces::~SUMOSAXAttributesImpl_Xerces() {
}


bool
SUMOSAXAttributesImpl_Xerces::hasAttribute(int id) const {
    AttrMap::const_iterator i = myPredefinedTags.find(id);
    if (i == myPredefinedTags.end()) {
        return false;
    }
    return myAttrs.getIndex((*i).second) >= 0;
}


bool
SUMOSAXAttributesImpl_Xerces::getBool(int id) const {
    return StringUtils::toBool(getString(id));
}


int
SUMOSAXAttributesImpl_Xerces::getInt(int id) const {
    return StringUtils::toInt(getString(id));
}


long long int
SUMOSAXAttributesImpl_Xerces::getLong(int id) const {
    return StringUtils::toLong(getString(id));
}


std::string
SUMOSAXAttributesImpl_Xerces::getString(int id) const {
    return StringUtils::transcode(getAttributeValueSecure(id));
}


std::string
SUMOSAXAttributesImpl_Xerces::getStringSecure(int id, const std::string& str) const {
    const XMLCh* utf16 = getAttributeValueSecure(id);
    if (XERCES_CPP_NAMESPACE::XMLString::stringLen(utf16) == 0) {
        // TranscodeToStr and debug_new interact badly in this case;
        return str;
    } else {
        return getString(id);
    }
}


double
SUMOSAXAttributesImpl_Xerces::getFloat(int id) const {
    return StringUtils::toDouble(getString(id));
}


const XMLCh*
SUMOSAXAttributesImpl_Xerces::getAttributeValueSecure(int id) const {
    AttrMap::const_iterator i = myPredefinedTags.find(id);
    assert(i != myPredefinedTags.end());
    return myAttrs.getValue((*i).second);
}


double
SUMOSAXAttributesImpl_Xerces::getFloat(const std::string& id) const {
    XMLCh* t = XERCES_CPP_NAMESPACE::XMLString::transcode(id.c_str());
    const std::string utf8 = StringUtils::transcode(myAttrs.getValue(t));
    XERCES_CPP_NAMESPACE::XMLString::release(&t);
    return StringUtils::toDouble(utf8);
}


bool
SUMOSAXAttributesImpl_Xerces::hasAttribute(const std::string& id) const {
    XMLCh* t = XERCES_CPP_NAMESPACE::XMLString::transcode(id.c_str());
    bool result = myAttrs.getIndex(t) >= 0;
    XERCES_CPP_NAMESPACE::XMLString::release(&t);
    return result;
}


std::string
SUMOSAXAttributesImpl_Xerces::getStringSecure(const std::string& id,
        const std::string& str) const {
    XMLCh* t = XERCES_CPP_NAMESPACE::XMLString::transcode(id.c_str());
    const XMLCh* v = myAttrs.getValue(t);
    XERCES_CPP_NAMESPACE::XMLString::release(&t);
    if (v == nullptr) {
        return str;
    } else {
        return StringUtils::transcode(v);
    }
}


SumoXMLEdgeFunc
SUMOSAXAttributesImpl_Xerces::getEdgeFunc(bool& ok) const {
    if (hasAttribute(SUMO_ATTR_FUNCTION)) {
        std::string funcString = getString(SUMO_ATTR_FUNCTION);
        if (SUMOXMLDefinitions::EdgeFunctions.hasString(funcString)) {
            return SUMOXMLDefinitions::EdgeFunctions.get(funcString);
        }
        ok = false;
    }
    return EDGEFUNC_NORMAL;
}


SumoXMLNodeType
SUMOSAXAttributesImpl_Xerces::getNodeType(bool& ok) const {
    if (hasAttribute(SUMO_ATTR_TYPE)) {
        std::string typeString = getString(SUMO_ATTR_TYPE);
        if (SUMOXMLDefinitions::NodeTypes.hasString(typeString)) {
            return SUMOXMLDefinitions::NodeTypes.get(typeString);
        }
        ok = false;
    }
    return NODETYPE_UNKNOWN;
}

RightOfWay
SUMOSAXAttributesImpl_Xerces::getRightOfWay(bool& ok) const {
    if (hasAttribute(SUMO_ATTR_RIGHT_OF_WAY)) {
        std::string rowString = getString(SUMO_ATTR_RIGHT_OF_WAY);
        if (SUMOXMLDefinitions::RightOfWayValues.hasString(rowString)) {
            return SUMOXMLDefinitions::RightOfWayValues.get(rowString);
        }
        ok = false;
    }
    return RIGHT_OF_WAY_DEFAULT;
}


RGBColor
SUMOSAXAttributesImpl_Xerces::getColor() const {
    return RGBColor::parseColor(getString(SUMO_ATTR_COLOR));
}


PositionVector
SUMOSAXAttributesImpl_Xerces::getShape(int attr) const {
    StringTokenizer st(getString(attr));
    PositionVector shape;
    while (st.hasNext()) {
        StringTokenizer pos(st.next(), ",");
        if (pos.size() != 2 && pos.size() != 3) {
            throw FormatException("shape format");
        }
        double x = StringUtils::toDouble(pos.next());
        double y = StringUtils::toDouble(pos.next());
        if (pos.size() == 2) {
            shape.push_back(Position(x, y));
        } else {
            double z = StringUtils::toDouble(pos.next());
            shape.push_back(Position(x, y, z));
        }
    }
    return shape;
}


Boundary
SUMOSAXAttributesImpl_Xerces::getBoundary(int attr) const {
    std::string def = getString(attr);
    StringTokenizer st(def, ",");
    if (st.size() != 4) {
        throw FormatException("boundary format");
    }
    const double xmin = StringUtils::toDouble(st.next());
    const double ymin = StringUtils::toDouble(st.next());
    const double xmax = StringUtils::toDouble(st.next());
    const double ymax = StringUtils::toDouble(st.next());
    return Boundary(xmin, ymin, xmax, ymax);
}


std::vector<std::string>
SUMOSAXAttributesImpl_Xerces::getStringVector(int attr) const {
    std::string def = getString(attr);
    std::vector<std::string> ret;
    parseStringVector(def, ret);
    return ret;
}


std::string
SUMOSAXAttributesImpl_Xerces::getName(int attr) const {
    if (myPredefinedTagsMML.find(attr) == myPredefinedTagsMML.end()) {
        return "?";
    }
    return myPredefinedTagsMML.find(attr)->second;
}


void
SUMOSAXAttributesImpl_Xerces::serialize(std::ostream& os) const {
    for (int i = 0; i < (int)myAttrs.getLength(); ++i) {
        os << " " << StringUtils::transcode(myAttrs.getLocalName(i));
        os << "=\"" << StringUtils::transcode(myAttrs.getValue(i)) << "\"";
    }
}


SUMOSAXAttributes*
SUMOSAXAttributesImpl_Xerces::clone() const {
    std::map<std::string, std::string> attrs;
    for (int i = 0; i < (int)myAttrs.getLength(); ++i) {
        attrs[StringUtils::transcode(myAttrs.getLocalName(i))] = StringUtils::transcode(myAttrs.getValue(i));
    }
    return new SUMOSAXAttributesImpl_Cached(attrs, myPredefinedTagsMML, getObjectType());
}

/****************************************************************************/

