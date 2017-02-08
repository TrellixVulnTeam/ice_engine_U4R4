#ifndef OPENGLLOADER_H_
#define OPENGLLOADER_H_

#include <mutex>
#include <deque>

#include "IOpenGlLoader.hpp"

namespace hercules
{

class OpenGlLoader : public IOpenGlLoader
{
public:
	OpenGlLoader();
	virtual ~OpenGlLoader();
	
	virtual void postWork(const std::function<void()>& work) override;
	virtual void waitAll() override;
	
	virtual unsigned int getWorkQueueCount() const override;
	
	virtual void tick() override;
	
	virtual void block() override;
	virtual void unblock() override;
	
private:
	mutable std::mutex enqueuedWorkMutex_;
	std::deque< std::function<void()> > enqueuedWork_;

	void initialize();
};

}

#endif /* OPENGLLOADER_H_ */
