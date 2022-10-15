#ifndef CONFIGEXPLANATION_H
#define CONFIGEXPLANATION_H


class ConfigExplanation
{
	public:
		ConfigExplanation();
		virtual ~ConfigExplanation();

		struct itemInfo
		{
			///name = value_name
			wxString name;
			///user_name = display_name (not mandatory)
			wxString user_name;
			///item help, usage
			wxString help;
			///item explanation
			wxString explanation;
//			value_type item_value_type;
			///P to value for item
			void *item_value=nullptr;
			///item_value_size useful for eg. qsort()
			BYTE item_value_size=1;
		};
		wxVector<itemInfo> ItemExplanation;

		size_t findBy_name( wxString search_name );
		size_t findBy_userName( wxString search_name );

		itemInfo getItemBy_id( size_t getBy_id ){
			return ItemExplanation.at(getBy_id);
		}

		itemInfo getItemBy_name( wxString search_name ){
			return getItemBy_id(findBy_name( search_name ));
		}
		itemInfo getItemBy_userName( wxString search_name ){
			return getItemBy_id(findBy_userName( search_name ));
		}

		bool exists_id(size_t getBy_id){
			if(getBy_id < ItemExplanation.size()) return true;
			return false;
		}

//

		void addItem( wxString name, wxString user_name="", wxString help="", wxString explanation="",
					void *item_value=nullptr, BYTE item_value_size=1 )
		{
			itemInfo tempIt;

			tempIt.name = name;
			tempIt.user_name = user_name;
			tempIt.help = help;
			tempIt.explanation = explanation;
//			value_type item_value_type;
			tempIt.item_value = item_value;
			tempIt.item_value_size = item_value_size;

			ItemExplanation.push_back(tempIt);
		}

		size_t findItemInString(wxString &findIn);


	protected:

	private:
};

#endif // CONFIGEXPLANATION_H
