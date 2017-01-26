namespace DORM {

	template<typename CXXTYPE>
	class SearchMod {
		private:
			bool p_is_set;
			CXXTYPE p_value;

		public:
			SearchMod(): p_is_set(false) {};

			void operator()(CXXTYPE value) { p_value = value; p_is_set = true; };
			CXXTYPE operator()() { return p_value; };

			operator bool() const { return p_is_set; };
	};
}
