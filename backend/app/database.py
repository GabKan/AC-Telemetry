from contextlib import contextmanager
from collections.abc import Generator
from typing import Annotated

from fastapi import Depends
from sqlmodel import Session, create_engine

from app.config import settings


engine = create_engine(
    settings.database_url,
    echo=settings.enable_database_logs,
    pool_pre_ping=True,
)


def check_database_connection() -> None:
    """Raise if the application cannot connect to the database."""
    with engine.connect() as connection:
        connection.exec_driver_sql("SELECT 1")


def _session_generator() -> Generator[Session, None, None]:
    with Session(engine) as session:
        try:
            yield session
        except Exception:
            session.rollback()
            raise


def get_session() -> Generator[Session, None, None]:
    yield from _session_generator()


@contextmanager
def get_cli_session() -> Generator[Session, None, None]:
    yield from _session_generator()


SessionDep = Annotated[Session, Depends(get_session)]
