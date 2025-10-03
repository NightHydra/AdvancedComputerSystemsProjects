def save_md_table_to_file(column_headers: dict[str, dict[str, str]], row_headers: dict[str, str],  savename: str):
    """

    :param column_headers: Each of the columns name, to what that columns data is
    :param row_headers: Actual Name matching to what its called in the data dictionaries
    :param savename: Filename to save the table as
    """

    # First Row
    table = ""
    table += "| |"
    for maj_cat in column_headers.keys():
        table += maj_cat + "|"
    table += "\n"

    # Second Row
    table += ("|" + "---|" * (len(column_headers.keys())+1)) + "\n"



    # Go through every row
    for row_name, row_key in row_headers.items():

        table += "|" + row_name + "|"

        for col_data in column_headers.values():

            # Get the mean of the column and print it
            table += f"{col_data[row_key]:.2f}"+"|"
        table += "\n"

    with open(savename, "w") as file:
        file.write(table)

