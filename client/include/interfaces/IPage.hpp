#pragma once

#ifndef _PAGE_NAME
#define _PAGE_NAME inline static const char* Name =
#endif // !_PAGE_NAME

class IPage
{
public:
    virtual ~IPage() = default;

    virtual void OnEnter() = 0;
    virtual void OnExit() = 0;
    virtual void Update() = 0;
    virtual void Render() = 0;

    virtual const char* GetTitle() const = 0;
    virtual bool        HasBadge() const = 0;
};