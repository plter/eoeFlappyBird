/* Copyright (c) 2012 Scott Lembcke and Howling Moon Software
 * Copyright (c) 2012 cocos2d-x.org
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "B2Sprite.h"
#include "support/CCPointExtension.h"

#include "Box2D/Box2D.h"

B2Sprite::B2Sprite()
: m_bIgnoreBodyRotation(false)
, m_pB2Body(NULL)
, m_fPTMRatio(0.0f)
{}

B2Sprite* B2Sprite::create()
{
    B2Sprite* pRet = new B2Sprite();
    if (pRet && pRet->init())
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }
    
    return pRet;
}

B2Sprite* B2Sprite::createWithTexture(CCTexture2D *pTexture)
{
    B2Sprite* pRet = new B2Sprite();
    if (pRet && pRet->initWithTexture(pTexture))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

B2Sprite* B2Sprite::createWithTexture(CCTexture2D *pTexture, const CCRect& rect)
{
    B2Sprite* pRet = new B2Sprite();
    if (pRet && pRet->initWithTexture(pTexture, rect))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

B2Sprite* B2Sprite::createWithSpriteFrame(CCSpriteFrame *pSpriteFrame)
{
    B2Sprite* pRet = new B2Sprite();
    if (pRet && pRet->initWithSpriteFrame(pSpriteFrame))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

B2Sprite* B2Sprite::createWithSpriteFrameName(const char *pszSpriteFrameName)
{
    B2Sprite* pRet = new B2Sprite();
    if (pRet && pRet->initWithSpriteFrameName(pszSpriteFrameName))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

B2Sprite* B2Sprite::create(const char *pszFileName)
{
    B2Sprite* pRet = new B2Sprite();
    if (pRet && pRet->initWithFile(pszFileName))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

B2Sprite* B2Sprite::create(const char *pszFileName, const CCRect& rect)
{
    B2Sprite* pRet = new B2Sprite();
    if (pRet && pRet->initWithFile(pszFileName, rect))
    {
        pRet->autorelease();
    }
    else
    {
        CC_SAFE_DELETE(pRet);
    }

    return pRet;
}

// this method will only get called if the sprite is batched.
// return YES if the physic's values (angles, position ) changed.
// If you return NO, then nodeToParentTransform won't be called.
bool B2Sprite::isDirty()
{
    return true;
}

bool B2Sprite::isIgnoreBodyRotation() const
{
    return m_bIgnoreBodyRotation;
}

void B2Sprite::setIgnoreBodyRotation(bool bIgnoreBodyRotation)
{
    m_bIgnoreBodyRotation = bIgnoreBodyRotation;
}

// Override the setters and getters to always reflect the body's properties.
const CCPoint& B2Sprite::getPosition()
{
    updatePosFromPhysics();
    return CCNode::getPosition();
}

void B2Sprite::getPosition(float* x, float* y)
{
    updatePosFromPhysics();
    return CCNode::getPosition(x, y);
}

float B2Sprite::getPositionX()
{
    updatePosFromPhysics();
    return m_obPosition.x;
}

float B2Sprite::getPositionY()
{
    updatePosFromPhysics();
    return m_obPosition.y;
}

b2Body* B2Sprite::getB2Body() const
{
    return m_pB2Body;
}

void B2Sprite::setB2Body(b2Body *pBody)
{
    m_pB2Body = pBody;
    pBody->SetUserData(this);
}

float B2Sprite::getPTMRatio() const
{
    return m_fPTMRatio;
}

void B2Sprite::setPTMRatio(float fRatio)
{
    m_fPTMRatio = fRatio;
}

// Override the setters and getters to always reflect the body's properties.
void B2Sprite::updatePosFromPhysics()
{
    b2Vec2 pos = m_pB2Body->GetPosition();
    float x = pos.x * m_fPTMRatio;
    float y = pos.y * m_fPTMRatio;
    m_obPosition = ccp(x,y);
}

void B2Sprite::setPosition(const CCPoint &pos)
{
    float angle = m_pB2Body->GetAngle();
    m_pB2Body->SetTransform(b2Vec2(pos.x / m_fPTMRatio, pos.y / m_fPTMRatio), angle);
}

float B2Sprite::getRotation()
{
    return (m_bIgnoreBodyRotation ? CCSprite::getRotation() :
            CC_RADIANS_TO_DEGREES(m_pB2Body->GetAngle()));
}

void B2Sprite::setRotation(float fRotation)
{
    if (m_bIgnoreBodyRotation)
    {
        CCSprite::setRotation(fRotation);
    }
    else
    {
        b2Vec2 p = m_pB2Body->GetPosition();
        float radians = CC_DEGREES_TO_RADIANS(fRotation);
        m_pB2Body->SetTransform(p, radians);
    }
}

// returns the transform matrix according the Box2D Body values
CCAffineTransform B2Sprite::nodeToParentTransform()
{
    b2Vec2 pos  = m_pB2Body->GetPosition();
	
	float x = pos.x * m_fPTMRatio;
	float y = pos.y * m_fPTMRatio;
	
	if (m_bIgnoreAnchorPointForPosition)
    {
		x += m_obAnchorPointInPoints.x;
		y += m_obAnchorPointInPoints.y;
	}
	
	// Make matrix
	float radians = m_pB2Body->GetAngle();
	float c = cosf(radians);
	float s = sinf(radians);
	
	// Although scale is not used by physics engines, it is calculated just in case
	// the sprite is animated (scaled up/down) using actions.
	// For more info see: http://www.cocos2d-iphone.org/forum/topic/68990
	if (!m_obAnchorPointInPoints.equals(CCPointZero))
    {
		x += ((c * -m_obAnchorPointInPoints.x * m_fScaleX) + (-s * -m_obAnchorPointInPoints.y * m_fScaleY));
		y += ((s * -m_obAnchorPointInPoints.x * m_fScaleX) + (c * -m_obAnchorPointInPoints.y * m_fScaleY));
	}
    
	// Rot, Translate Matrix
	m_sTransform = CCAffineTransformMake( c * m_fScaleX,	s * m_fScaleX,
									     -s * m_fScaleY,	c * m_fScaleY,
									     x,	y );
	
	return m_sTransform;
}
