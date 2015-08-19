#include "Cortica.h"

int
main (int argc, char ** argv) {
	Cortica* api = Cortica::GetApi (CORTICAPI_PROVIDER_CLOUD);
	api->initializeDB ();

	RGBImage data;
	data.ImageURL = "http://media.cmgdigital.com/shared/img/photos/2012/08/13/7f/f8/slideshow_782633_carmarket0310c.JPG";
	vector<Tag> tags = api->matchingSync (&data);

	for (vector<Tag>::iterator item = tags.begin(); item != tags.end(); ++item) {
		std::cout << (*item).Name << std::endl;
	}

	std::cout << "Exit 'cortica-test'" << std::endl;
	return 0;
}