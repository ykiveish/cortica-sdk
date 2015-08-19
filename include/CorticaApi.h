class CorticaApi {
public:
	static Cortica* getApi (CORTICAPI_PROVIDER provider) {
		if (!m_instance) {
			m_instance = new Cortica ();
			m_instance->SetProvider (provider);
			return m_instance;
		}

		return m_instance;
	}

	void freeApi () {
		delete m_instance;
	}

private:
	CorticaApi () { }
	CorticaApi (CorticaApi const&) 		= delete;
	void operator= (CorticaApi const&) 	= delete;

	static Cortica* m_instance;
};
